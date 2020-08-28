#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "dbman.cpp"
#include <QButtonGroup>
#include <QRegExp>
#include <QValidator>
#include <QDebug>



settingsWidget::settingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingsWidget)
{
    ui->setupUi(this);

    /*
     * General Tab
     */
    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox);
    themeGroup->addButton(ui->lightThemeCheckBox);
    themeGroup->addButton(ui->darkThemeCheckBox);

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

    QRegExp flightNumberPrefix_rx("[a-zA-Z]?[a-zA-Z]?[a-zA-Z]?[a-zA-Z]"); // allow max 4 letters (upper and lower)
    QValidator *flightNumberPrefixValidator = new QRegExpValidator(flightNumberPrefix_rx, this);
    ui->flightNumberPrefixLineEdit->setValidator(flightNumberPrefixValidator);

}

settingsWidget::~settingsWidget()
{
    delete ui;
}

/*
 * General Tab
 */

void settingsWidget::on_flightNumberPrefixLineEdit_textEdited(const QString &arg1)
{
    dbSettings::storeSetting(50, arg1);
}
