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




settingsWidget::settingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingsWidget)
{
    ui->setupUi(this);

    /*
     * General Tab
     */
    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);
    connect(themeGroup, SIGNAL(buttonClicked(int)), this, SLOT(themeGroup_toggled(int)));

    switch (dbSettings::retreiveSetting(10).toInt()) {
      case 0:
        qDebug() << "System Theme";
        ui->systemThemeCheckBox->setChecked(true);
        break;
      case 1:
        qDebug() << "Light Theme";
        ui->lightThemeCheckBox->setChecked(true);
        break;
      case 2:
        qDebug() << "Dark Theme";
        ui->darkThemeCheckBox->setChecked(true);
    }
    /*
     * Flight Logging Tab
     */
    QString storedPrefix = dbSettings::retreiveSetting(50);
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
    dbSettings::storeSetting(50, arg1);
}

void settingsWidget::themeGroup_toggled(int id)
{
    dbSettings::storeSetting(10,QString::number(id));

    QMessageBox *info = new QMessageBox(this);
    info->setText("Theme change will take effect next time you start the application.");
    info->exec();
}

void settingsWidget::on_aboutPushButton_clicked()
{
    auto mb = new QMessageBox(this);
    QString SQLITE_VERSION = dbSettings::sqliteversion();
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
