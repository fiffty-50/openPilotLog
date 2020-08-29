#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "dbman.cpp"
#include <QButtonGroup>
#include <QRegExp>
#include <QValidator>
#include <QMessageBox>
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
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);

    /*connect(themeGroup,
            SIGNAL(idToggled(int)),
            this,
            SLOT(themeGroup_toggled(int)));*/

    /*connect(themeGroup,
    SIGNAL(idToggled(int)),
    SLOT(themeGroup_toggled(int)));*/

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

    QRegExp flightNumberPrefix_rx("[a-zA-Z]?[a-zA-Z]?[a-zA-Z]?[a-zA-Z]"); // allow max 4 letters (upper and lower)
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
