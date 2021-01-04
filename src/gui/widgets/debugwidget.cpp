#include "debugwidget.h"
#include "ui_debugwidget.h"
#include "src/classes/astandardpaths.h"
#include "src/gui/widgets/logbookwidget.h"
#include "src/gui/widgets/pilotswidget.h"
#include "src/gui/widgets/aircraftwidget.h"

DebugWidget::DebugWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWidget)
{
    ui->setupUi(this);
    for (const auto& table : aDB()->getTableNames()) {
        if( table != "sqlite_sequence") {
            ui->tableComboBox->addItem(table);
        }
    }
}

DebugWidget::~DebugWidget()
{
    delete ui;
}

void DebugWidget::on_resetUserTablesPushButton_clicked()
{
    ATimer timer(this);
    QMessageBox result;
    if (ADataBaseSetup::resetToDefault()){
        result.setText("Database successfully reset");
        result.exec();
        emit aDB()->dataBaseUpdated();
    } else {
        result.setText("Errors have occurred. Check console for Debug output. ");
        result.exec();
    }
}

void DebugWidget::on_resetDatabasePushButton_clicked()
{
    ATimer timer(this);
    QMessageBox message_box(this);

    // download latest csv
    QString link_stub = "https://raw.githubusercontent.com/fiffty-50/openpilotlog/";
    link_stub.append(ui->branchLineEdit->text()); // optionally select branch for development
    link_stub.append("/assets/database/templates/");

    QStringList template_tables = {"aircraft", "airports", "changelog"};
    QDir template_dir(AStandardPaths::absPathOf(AStandardPaths::Templates));
    for (const auto& table : template_tables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        QObject::connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(link_stub % table % QStringLiteral(".csv")));
        dl->setFileName(template_dir.filePath(table % QStringLiteral(".csv")));
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }

    // back up old db
    aDB()->disconnect();
    ADataBaseSetup::backupOldData();

    // re-connct and create new database
    aDB()->connect();
    if (ADataBaseSetup::createDatabase()) {
        DEB << "Database has been successfully created.";
    } else {
        message_box.setText("Errors have ocurred creating the database.<br>"
                            "Check console for details.");
        message_box.exec();
    }
    if (ADataBaseSetup::importDefaultData()) {
        message_box.setText("Database has been successfully reset.");
        emit aDB()->dataBaseUpdated();
        message_box.exec();
    } else {
        message_box.setText("Errors have ocurred while importing templates.<br>"
                            "Check console for details.");
        message_box.exec();
    }
}

void DebugWidget::downloadFinished()
{

}

void DebugWidget::on_fillUserDataPushButton_clicked()
{
    ATimer timer(this);
    QMessageBox message_box(this);
    // download latest csv
    QStringList userTables = {"pilots", "tails", "flights"};
    QString linkStub = "https://raw.githubusercontent.com/fiffty-50/openpilotlog/";
    linkStub.append(ui->branchLineEdit->text());
    linkStub.append("/assets/database/templates/sample_");
    QDir template_dir(AStandardPaths::absPathOf(AStandardPaths::Templates));

    for (const auto& table : userTables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(linkStub + table + ".csv"));
        dl->setFileName(template_dir.filePath("sample_" + table % QStringLiteral(".csv")));
        //dl->setFileName("data/templates/sample_" + table + ".csv");
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }
    QBitArray allGood;
    allGood.resize(userTables.size());

    for (const auto& table : userTables) {
        auto data = aReadCsv(AStandardPaths::absPathOf(AStandardPaths::Templates)
                             + "/sample_" + table + ".csv");
        allGood.setBit(userTables.indexOf(table), ADataBaseSetup::commitData(data, table));
    }

    if (allGood.count(true) != userTables.size()) {
        message_box.setText("Errors have ocurred. Check console for details.");
        message_box.exec();
        return;
    }

    message_box.setText("User tables successfully populated.");
    message_box.exec();
    emit aDB()->dataBaseUpdated();
}

void DebugWidget::on_selectCsvPushButton_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open CSV File for import"),
                                                 AStandardPaths::absPathOf(AStandardPaths::Templates),
                                                 tr("CSV files (*.csv)"));
    ui->importCsvLineEdit->setText(fileName);
}

void DebugWidget::on_importCsvPushButton_clicked()
{
    ATimer timer(this);
    auto file = QFileInfo(ui->importCsvLineEdit->text());
    DEB << "File exists/is file: " << file.exists() << file.isFile() << " Path: " << file.absoluteFilePath();

    if (file.exists() && file.isFile()) {

        if (ADataBaseSetup::commitData(aReadCsv(file.absoluteFilePath()), ui->tableComboBox->currentText())) {
            auto mb = QMessageBox(this);
            mb.setText("Data inserted successfully.");
            mb.exec();
        } else {
            auto mb = QMessageBox(this);
            mb.setText("Errors have ocurred. Check console for details.");
            mb.exec();
        }
    } else {
        auto mb = QMessageBox(this);
        mb.setText("Please select a valid file.");
        mb.exec();
    }
}

void DebugWidget::on_debugPushButton_clicked()
{

}

/* //Comparing two functions template
    qlonglong number_of_runs = 5000;
    long time1 = 0;
    long time2 = 0;
    {

        ATimer timer;
        for (int i = 0; i < number_of_runs; i++) {
            // first block, do stuff here...
        }

        time1 = timer.timeNow();
    }
    {
        ATimer timer;
        for (int i = 0; i < number_of_runs; i++) {
            // second block, do stuff here...
        }
        time2 = timer.timeNow();
    }

    DEB << "First block executed " << number_of_runs << " times for a total of " << time1 << " milliseconds.");
    DEB << "Second block executed " << number_of_runs << " times for a total of " << time2 << " milliseconds.");
*/




/*qlonglong number_of_runs = 500;
        long time1 = 0;
        {

            ATimer timer;
            for (int i = 0; i < number_of_runs; i++) {
                // first block, do stuff here...
                auto acft = aDB()->getTailEntry(5);
                auto pilot = aDB()->getPilotEntry(7);
                auto flight = aDB()->getFlightEntry(15);
                QList<AEntry> list = {acft, pilot, flight};
                for (auto entry : list) {
                    for (auto column : entry.getData()) {
                        QString value = column.toString();
                    }
                }
            }

            time1 = timer.timeNow();
        }

        DEB << "First block executed " << number_of_runs << " times for a total of " << time1 << " milliseconds.");
        // 108 - 134 milliseconds with legacy exp db api
        // 108 - 110 milliseconds with improved exp api
        // to do: with string literals*/
