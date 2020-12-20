#include "debugwidget.h"
#include "ui_debugwidget.h"

using namespace experimental;

DebugWidget::DebugWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWidget)
{
    ui->setupUi(this);
    for (const auto& table : DbInfo().tables) {
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
        result.setText("Database successfully reset.\n\nRestarting app.");
        result.exec();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    } else {
        result.setText("Errors have occurred. Check console for Debug output. ");
        result.exec();
    }
}

void DebugWidget::on_resetDatabasePushButton_clicked()
{
    ATimer timer(this);
    QMessageBox mb(this);
    //check if template dir exists and create if needed.
    QDir dir("data/templates");
    DEB(dir.path());
    if (!dir.exists())
        dir.mkpath(".");
    // download latest csv
    QStringList templateTables = {"aircraft", "airports", "changelog"};
    QString linkStub = "https://raw.githubusercontent.com/fiffty-50/openpilotlog/";
    linkStub.append(ui->branchLineEdit->text());
    linkStub.append("/assets/database/templates/");
    for (const auto& table : templateTables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(linkStub + table + ".csv"));
        dl->setFileName("data/templates/" + table + ".csv");
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }
    //close database connection
    aDB()->disconnect();

    // back up old database
    auto oldDatabase = QFile("data/logbook.db");
    if (oldDatabase.exists()) {
        auto dateString = QDateTime::currentDateTime().toString(Qt::ISODate);
        DEB("Backing up old database as: " << "logbook-backup-" + dateString);
        if (!oldDatabase.rename("data/logbook-backup-" + dateString)) {
            DEB("Warning: Creating backup of old database has failed.");
        }
    }
    // re-connct and create new database
    aDB()->connect();

    if (ADataBaseSetup::createDatabase()) {
        mb.setText("Database has been successfully reset.\n\nRestarting application.");
        mb.exec();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    } else {
        mb.setText("Errors have ocurred. Check console for details.");
    }
}

void DebugWidget::downloadFinished()
{

}

void DebugWidget::on_fillUserDataPushButton_clicked()
{
    ATimer timer(this);
    QMessageBox mb(this);
    //check if template dir exists and create if needed.
    QDir dir("data/templates");
    DEB(dir.path());
    if (!dir.exists())
        dir.mkpath(".");
    // download latest csv
    QStringList userTables = {"pilots", "tails", "flights"};
    QString linkStub = "https://raw.githubusercontent.com/fiffty-50/openpilotlog/";
    linkStub.append(ui->branchLineEdit->text());
    linkStub.append("/assets/database/templates/sample_");

    for (const auto& table : userTables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(linkStub + table + ".csv"));
        dl->setFileName("data/templates/sample_" + table + ".csv");
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }
    QBitArray allGood;
    allGood.resize(userTables.size());

    for (const auto& table : userTables) {
        auto data = aReadCsv("data/templates/sample_" + table + ".csv");
        allGood.setBit(userTables.indexOf(table), ADataBaseSetup::commitData(data, table));
    }

    if (allGood.count(true) != userTables.size()) {
        mb.setText("Errors have ocurred. Check console for details.");
        mb.exec();
        return;
    }

    mb.setText("User tables successfully populated.\n\nRestarting app.");
    mb.exec();
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void DebugWidget::on_selectCsvPushButton_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open CSV File for import"), QDir::homePath(), tr("CSV files (*.csv)"));
    ui->importCsvLineEdit->setText(fileName);
}

void DebugWidget::on_importCsvPushButton_clicked()
{
    ATimer timer(this);
    auto file = QFileInfo(ui->importCsvLineEdit->text());
    DEB("File exists/is file: " << file.exists() << file.isFile() << " Path: " << file.absoluteFilePath());

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
    using namespace experimental;

    auto acft = aDB()->getTailEntry(5);
    DEB(acft.getData().key("make"));
    DEB(acft.type());


}

/* //Comparing two functions template
    qlonglong number_of_runs = 5000;
    long time1 = 0;
    long time2 = 0;
    using namespace experimental;
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

    DEB("First block executed " << number_of_runs << " times for a total of " << time1 << " milliseconds.");
    DEB("Second block executed " << number_of_runs << " times for a total of " << time2 << " milliseconds.");
*/
