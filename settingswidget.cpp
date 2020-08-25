#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "dbman.cpp"
#include <QButtonGroup>
#include <QDebug>



settingsWidget::settingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingsWidget)
{
    ui->setupUi(this);

    auto themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox);
    themeGroup->addButton(ui->lightThemeCheckBox);
    themeGroup->addButton(ui->darkThemeCheckBox);

    int themeSetting = db::retreiveSetting("10").toInt();
    qDebug() << themeSetting;
    switch (themeSetting) {
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
}

settingsWidget::~settingsWidget()
{
    delete ui;
}
