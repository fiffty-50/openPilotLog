/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "debugwidget.h"
#include "src/gui/verification/airportinput.h"
#include "src/gui/verification/completerprovider.h"
#include "src/gui/verification/pilotinput.h"
#include "src/gui/verification/timeinput.h"
#include "src/testing/importCrewlounge/processaircraft.h"
#include "src/testing/importCrewlounge/processflights.h"
#include "src/testing/importCrewlounge/processpilots.h"
#include "ui_debugwidget.h"
#include <QtGlobal>
#include "src/classes/downloadhelper.h"
#include "src/functions/readcsv.h"
#include "src/database/database.h"
#include "src/database/row.h"
#include "src/testing/atimer.h"
#include "src/functions/log.h"
void DebugWidget::on_debugPushButton_clicked()
{
    auto rawCsvData = CSV::readCsvAsRows("/home/felix/git/importMCC/assets/data/felix.csv");
    // Process Pilots
    auto proc_pilots = ProcessPilots(rawCsvData);
    proc_pilots.init();
    const auto p_maps = proc_pilots.getProcessedPilotMaps();
    // Process Tails
    auto proc_tails = ProcessAircraft(rawCsvData);
    proc_tails.init();
    const auto t_maps = proc_tails.getProcessedTailMaps();
    // Process Flights
    auto proc_flights = ProcessFlights(rawCsvData,proc_pilots.getProcessedPilotsIds(), proc_tails.getProcessedTailIds());
    proc_flights.init();

    auto flights = proc_flights.getProcessedFlights();
    DEB << "Flight:" << flights[1000];
}

DebugWidget::DebugWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWidget)
{
    ui->setupUi(this);
    for (const auto& table : DB->getTableNames()) {
        if( table != "sqlite_sequence") {
            ui->tableComboBox->addItem(table);
        }
    }
    ui->debugLineEdit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Airports));
    ui->debug2LineEdit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Airports));
}

DebugWidget::~DebugWidget()
{
    delete ui;
}

void DebugWidget::on_resetUserTablesPushButton_clicked()
{
    ATimer timer(this);
    if (DB->resetUserData()){
        LOG << "Database successfully reset";
        emit DB->dataBaseUpdated(OPL::DbTable::Any);
    } else
        LOG <<"Errors have occurred. Check console for Debug output. ";
}

void DebugWidget::on_resetDatabasePushButton_clicked()
{
    // disconnect and remove old database
    DB->disconnect();
    QFile db_file(OPL::Paths::databaseFileInfo().absoluteFilePath());
    if (!db_file.remove()) {
        WARN(tr("Unable to delete existing database file."));
        return;
    }


    // Download templates
    QString branch_name = ui->branchLineEdit->text();

    // Create url string
    auto template_url_string = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/");
    template_url_string.append(branch_name);
    template_url_string.append(QLatin1String("/assets/database/templates/"));

    QDir template_dir(OPL::Paths::directory(OPL::Paths::Templates));
    QStringList template_table_names;
    for (const auto table : DB->getTemplateTables())
        template_table_names.append(OPL::GLOBALS->getDbTableName(table));
    // Download json files
    for (const auto& table_name : template_table_names) {
        QEventLoop loop;
        DownloadHelper* dl = new DownloadHelper;
        QObject::connect(dl, &DownloadHelper::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(template_url_string + table_name + QLatin1String(".json")));
        dl->setFileName(template_dir.absoluteFilePath(table_name + QLatin1String(".json")));
        DEB << "Downloading: " << template_url_string + table_name + QLatin1String(".json");
        dl->download();
        dl->deleteLater();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue

        QFileInfo downloaded_file(template_dir.filePath(table_name + QLatin1String(".json")));
        if (downloaded_file.size() == 0)
            LOG << "ssl/network error";
    }
    // Download checksum files
    for (const auto& table : template_table_names) {
        QEventLoop loop;
        DownloadHelper* dl = new DownloadHelper;
        QObject::connect(dl, &DownloadHelper::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(template_url_string + table + QLatin1String(".md5")));
        dl->setFileName(template_dir.absoluteFilePath(table + QLatin1String(".md5")));

        DEB << "Downloading: " << template_url_string + table + QLatin1String(".md5");

        dl->download();
        dl->deleteLater();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue

        QFileInfo downloaded_file(template_dir.filePath(table + QLatin1String(".md5")));
        if (downloaded_file.size() == 0)
            LOG << "ssl/network error";
    }
    // Create Database
    if (!DB->createSchema()) {
        WARN(QString("Unable to create database.<br>%1").arg(DB->lastError.text()));
        return;
    }

    // Load ressources
    bool use_ressource_data = false; // do not use local data, download from github
    if(!DB->importTemplateData(use_ressource_data)) {
        WARN(tr("Database creation has been unsuccessful. Unable to fill template data.<br><br>%1")
             .arg(DB->lastError.text()));
        return ;
    }

    DB->connect();

}

void DebugWidget::downloadFinished()
{

}

void DebugWidget::on_fillUserDataPushButton_clicked()
{
    TODO << "Create JSON sample data and import";
    /*
    ATimer timer(this);
    QMessageBox message_box(this);
    // download latest json
    QStringList userTables = {"pilots", "tails", "flights"};
    QString linkStub = "https://raw.githubusercontent.com/fiffty-50/openpilotlog/";
    linkStub.append(ui->branchLineEdit->text());
    linkStub.append("/assets/database/templates/sample_");
    QDir template_dir(AStandardPaths::directory(AStandardPaths::Templates));

    for (const auto& table : userTables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(linkStub + table + ".json"));
        dl->setFileName(template_dir.filePath("sample_" % table % QStringLiteral(".json")));
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }
    QBitArray allGood;
    allGood.resize(userTables.size());

    for (const auto& table : userTables) {
        auto data = readcsv(AStandardPaths::directory(AStandardPaths::Templates).absoluteFilePath(
                             + "sample_" + table + ".json"));
        allGood.setBit(userTables.indexOf(table), aDbSetup::commitData(data, table));
    }

    if (allGood.count(true) != userTables.size()) {
        message_box.setText("Errors have ocurred. Check console for details.");
        message_box.exec();
        return;
    }

    message_box.setText("User tables successfully populated.");
    message_box.exec();
    emit DB->dataBaseUpdated();
    */
}

void DebugWidget::on_selectCsvPushButton_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open CSV File for import"),
                                                 OPL::Paths::directory(OPL::Paths::Templates).absolutePath(),
                                                 tr("CSV files (*.csv)"));
    ui->importCsvLineEdit->setText(fileName);
}

void DebugWidget::on_importCsvPushButton_clicked()
{
    WARN("Not currently working...");
    /*
    ATimer timer(this);
    auto file = QFileInfo(ui->importCsvLineEdit->text());
    DEB << "File exists/is file:" << file.exists() << file.isFile() << " Path:" << file.absoluteFilePath();

    if (file.exists() && file.isFile()) {

        if (DataBaseSetup::commitData(aReadCsv(file.absoluteFilePath()), ui->tableComboBox->currentText())) {
            QMessageBox message_box(this);
            message_box.setText("Data inserted successfully.");
            message_box.exec();
        } else {
            QMessageBox message_box(this);
            message_box.setText("Errors have ocurred. Check console for details.");
            message_box.exec();
        }
    } else {
        QMessageBox message_box(this);
        message_box.setText("Please select a valid file.");
        message_box.exec();
    }
    */
}

void DebugWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
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

/*
 *#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
 *   DEB << QT_VERSION_MAJOR << QT_VERSION_MINOR << "At least 5.12";
 *#else
 *   DEB << QT_VERSION_MAJOR << QT_VERSION_MINOR << "Less than 5.12";
 * #endif
 */


void DebugWidget::on_debugLineEdit_editingFinished()
{
    PilotInput user_input = PilotInput(ui->debugLineEdit->text());
    if(user_input.isValid())
        DEB << "Good Input";
    else {
        DEB << "Fixing...";
        ui->debugLineEdit->setText(user_input.fixup());
    }
}


void DebugWidget::on_debug2LineEdit_editingFinished()
{

}

