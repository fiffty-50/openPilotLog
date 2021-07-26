/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include "ui_debugwidget.h"
#include "src/classes/astandardpaths.h"
#include "src/gui/widgets/logbookwidget.h"
#include "src/gui/widgets/pilotswidget.h"
#include "src/gui/widgets/aircraftwidget.h"
#include "src/gui/dialogues/firstrundialog.h"
#include "src/gui/widgets/backupwidget.h"
#include <QtGlobal>
#include "src/functions/atime.h"
#include "src/functions/astat.h"
#include "src/classes/acurrencyentry.h"
#include "src/classes/atranslator.h"
#include "src/database/adatabasesetup.h"
#include "src/classes/ahash.h"

#include "src/classes/ajson.h"
void DebugWidget::on_debugPushButton_clicked()
{
    // Debug
    QFileInfo check_file("/home/felix/.local/share/opl/openPilotLog/templates/changelog.json");
    AHash hash(check_file);

    QFileInfo md5_file("/home/felix/.local/share/opl/openPilotLog/templates/changelog.md5");
    DEB << "Sums are equal?" << hash.compare(md5_file);
    //test_file2.open(QFile::ReadOnly);
    //QTextStream in(&test_file2);
    //auto read = in.read(32);
    //auto array = read.toUtf8();
    //test_file2.close();
    //DEB << read;
    //DEB << (read == hash.hashToHex());
    //DEB << array;


    //for (const auto &table_name : aDB->getTemplateTableNames()) {
    //    //json_files.append(QFile(AStandardPaths::asChildOfDir(AStandardPaths::Templates, table_name)));
    //    QString json_path = AStandardPaths::asChildOfDir(AStandardPaths::Templates, table_name) + QLatin1String(".json");
    //    QString md5_path = AStandardPaths::asChildOfDir(AStandardPaths::Templates, table_name) + QLatin1String(".md5");
    //    DEB << json_path << md5_path;
    //    QFileInfo json_fi(json_path);
    //    QFileInfo md5_fi(md5_path);
    //    DEB << "Exists? " << json_fi.exists() << md5_fi.exists();
    //}

}

DebugWidget::DebugWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWidget)
{
    ui->setupUi(this);
    for (const auto& table : aDB->getTableNames()) {
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
    QMessageBox message_box(this);
    if (ADataBaseSetup::resetToDefault()){
        message_box.setText("Database successfully reset");
        message_box.exec();
        emit aDB->dataBaseUpdated();
    } else {
        message_box.setText("Errors have occurred. Check console for Debug output. ");
        message_box.exec();
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
    QDir template_dir(AStandardPaths::directory(AStandardPaths::Templates));
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
    aDB->disconnect();
    ADataBaseSetup::backupOldData();

    // re-connct and create new database
    aDB->connect();
    if (ADataBaseSetup::createDatabase()) {
        DEB << "Database has been successfully created.";
    } else {
        message_box.setText("Errors have ocurred creating the database.<br>"
                            "Check console for details.");
        message_box.exec();
    }
    if (ADataBaseSetup::importDefaultData(false)) {
        message_box.setText("Database has been successfully reset.");
        emit aDB->dataBaseUpdated();
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
    QDir template_dir(AStandardPaths::directory(AStandardPaths::Templates));

    for (const auto& table : userTables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(linkStub + table + ".csv"));
        dl->setFileName(template_dir.filePath("sample_" % table % QStringLiteral(".csv")));
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }
    QBitArray allGood;
    allGood.resize(userTables.size());

    for (const auto& table : userTables) {
        auto data = aReadCsv(AStandardPaths::directory(AStandardPaths::Templates).absoluteFilePath(
                             + "sample_" + table + ".csv"));
        allGood.setBit(userTables.indexOf(table), ADataBaseSetup::commitData(data, table));
    }

    if (allGood.count(true) != userTables.size()) {
        message_box.setText("Errors have ocurred. Check console for details.");
        message_box.exec();
        return;
    }

    message_box.setText("User tables successfully populated.");
    message_box.exec();
    emit aDB->dataBaseUpdated();
}

void DebugWidget::on_selectCsvPushButton_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open CSV File for import"),
                                                 AStandardPaths::directory(AStandardPaths::Templates).absolutePath(),
                                                 tr("CSV files (*.csv)"));
    ui->importCsvLineEdit->setText(fileName);
}

void DebugWidget::on_importCsvPushButton_clicked()
{
    ATimer timer(this);
    auto file = QFileInfo(ui->importCsvLineEdit->text());
    DEB << "File exists/is file:" << file.exists() << file.isFile() << " Path:" << file.absoluteFilePath();

    if (file.exists() && file.isFile()) {

        if (ADataBaseSetup::commitData(aReadCsv(file.absoluteFilePath()), ui->tableComboBox->currentText())) {
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

