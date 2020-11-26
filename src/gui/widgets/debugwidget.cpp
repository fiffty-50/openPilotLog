#include "debugwidget.h"
#include "ui_debugwidget.h"
#include "debug.h"

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
    QMessageBox result;
    if (DbSetup::resetToDefault()){
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
    QMessageBox mb(this);
    //check if template dir exists and create if needed.
    QDir dir("data/templates");
    DEB(dir.path());
    if (!dir.exists())
        dir.mkpath(".");
    // download latest csv
    QStringList templateTables = {"aircraft", "airports", "changelog"};
    const auto& linkStub = "https://raw.githubusercontent.com/fiffty-50/openpilotlog/devel/assets/database/templates/";
    for (const auto& table : templateTables) {
        QEventLoop loop;
        Download* dl = new Download;
        connect(dl, &Download::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(linkStub + table + ".csv"));
        dl->setFileName("data/templates/" + table + ".csv");
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }
    //close database connection
    Db::disconnect();

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
    Db::connect();

    if (DbSetup::createDatabase()) {
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
    QMessageBox mb(this);
    //check if template dir exists and create if needed.
    QDir dir("data/templates");
    DEB(dir.path());
    if (!dir.exists())
        dir.mkpath(".");
    // download latest csv
    QStringList userTables = {"pilots", "tails", "flights"};
    const auto& linkStub = "https://raw.githubusercontent.com/fiffty-50/openpilotlog/devel/assets/database/templates/sample_";
    for (const auto& table : userTables) {
        QEventLoop loop;
        Download* dl = new Download;
        connect(dl, &Download::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(linkStub + table + ".csv"));
        dl->setFileName("data/templates/sample_" + table + ".csv");
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }
    QVector<bool> allGood;
    for (const auto& table : userTables) {
        auto data = Csv::read("data/templates/sample_" + table + ".csv");
        allGood.append(DbSetup::commitData(data, table));
    }

    for (const auto& item : allGood) {
        if (!item) {
            mb.setText("Errors have ocurred. Check console for details.");
            mb.exec();
            return;
        }

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
    auto file = QFileInfo(ui->importCsvLineEdit->text());
    DEB("File exists/is file: " << file.exists() << file.isFile() << " Path: " << file.absoluteFilePath());

    if (file.exists() && file.isFile()) {

        if (DbSetup::commitData(Csv::read(file.absoluteFilePath()), ui->tableComboBox->currentText())) {
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
