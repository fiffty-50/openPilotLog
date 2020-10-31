/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "src/database/dbinfo.h"



settingsWidget::settingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingsWidget)
{
    ui->setupUi(this);
    QSettings settings;

    /*
     * General Tab
     */
    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);
    connect(themeGroup, SIGNAL(buttonClicked(int)), this, SLOT(themeGroup_toggled(int)));


    switch (settings.value("main/theme").toInt()) {
      case 0:
        ui->systemThemeCheckBox->setChecked(true);
        break;
      case 1:
        ui->lightThemeCheckBox->setChecked(true);
        break;
      case 2:
        ui->darkThemeCheckBox->setChecked(true);
    }
    /*
     * Flight Logging Tab
     */
    //QString storedPrefix = db::singleSelect("setting","settings","setting_id","50",sql::exactMatch);
    QString storedPrefix = settings.value("userdata/flightnumberPrefix").toString();
    if (storedPrefix.length() != 0){
        ui->flightNumberPrefixLineEdit->setText(storedPrefix);
    }

    QRegExp flightNumberPrefix_rx("[a-zA-Z0-9]?[a-zA-Z0-9]?[a-zA-Z0-9]"); // allow max 3 letters (upper and lower) and numbers
    QValidator *flightNumberPrefixValidator = new QRegExpValidator(flightNumberPrefix_rx, this);
    ui->flightNumberPrefixLineEdit->setValidator(flightNumberPrefixValidator);

}

settingsWidget::~settingsWidget()
{
    delete ui;
}

/*
 * Slots
 */

void settingsWidget::on_flightNumberPrefixLineEdit_textEdited(const QString &arg1)
{
    QSettings settings;
    settings.setValue("userdata/flightnumberPrefix",arg1);
}

void settingsWidget::themeGroup_toggled(int id)
{
    QSettings settings;
    settings.setValue("main/theme",id);


    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Changing Themes", "Changing the theme requires restarting the Application.\n\nWould you like to restart now?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());

    }else{
        QMessageBox *info = new QMessageBox(this);
        info->setText("Theme change will take effect the next time you start the application.");
        info->exec();
    }
}

void settingsWidget::on_aboutPushButton_clicked()
{
    auto mb = new QMessageBox(this);
    QString SQLITE_VERSION = dbInfo().version;
    QString text = QMessageBox::tr(

                "<h3><center>About openPilotLog</center></h3>"
                "<br>"
                "(c) 2020 Felix Turowsky"
                "<br>"
                "<p>This is a collaboratively developed Free and Open Source Application. "
                "Visit us <a href=\"https://%1/\">here</a> for more information.</p>"

                "<p>This program is free software: you can redistribute it and/or modify "
                "it under the terms of the GNU General Public License as published by "
                "the Free Software Foundation, either version 3 of the License, or "
                "(at your option) any later version.</p>"

                "<p>This program is distributed in the hope that it will be useful, "
                "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                "GNU General Public License for more details.</p> "

                "<p>You should have received a copy of the GNU General Public License "
                "along with this program.  If not, "
                "please click <a href=\"https://www.gnu.org/licenses/\">here</a>.</p>"

                "<br>"

                "<p>This program uses <a href=\"http://%2/\">Qt</a> version %3 and "
                "<a href=\"https://sqlite.org/about.html/\">SQLite</a> version %4</p>"
                ).arg(QLatin1String("github.com/fiffty-50/openpilotlog"),
                      QLatin1String("qt.io"),
                      QLatin1String(QT_VERSION_STR),
                      QString(SQLITE_VERSION));
    mb->setText(text);
    mb->open();
}
