#include "firstrundialog.h"
#include "ui_firstrundialog.h"

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->piclastnameLineEdit->setFocus();
    ui->nightComboBox->setCurrentIndex(1);

    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);
    connect(themeGroup, SIGNAL(buttonClicked(int)), this, SLOT(themeGroup_toggled(int)));
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::on_previousPushButton_clicked()
{
    if(ui->tabWidget->currentIndex()>0)
        ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()-1);
}

void FirstRunDialog::on_nextPushButton_clicked()
{
    if(ui->tabWidget->currentIndex()<2)
        ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()+1);
}

void FirstRunDialog::themeGroup_toggled(int id)
{
    QSettings settings;
    settings.setValue("main/theme", id);
}

void FirstRunDialog::on_finishButton_clicked()
{
    if(ui->piclastnameLineEdit->text().isEmpty() || ui->picfirstnameLineEdit->text().isEmpty()){
        auto mb = new QMessageBox(this);
        mb->setText("You have to enter a valid first and last name for the logbook.");
        mb->show();
    } else {
        QSettings settings;
        settings.setValue("userdata/piclastname",ui->piclastnameLineEdit->text());
        settings.setValue("userdata/picfirstname",ui->picfirstnameLineEdit->text());
        settings.setValue("userdata/employeeid",ui->employeeidLineEdit->text());
        settings.setValue("userdata/phone",ui->phoneLineEdit->text());
        settings.setValue("userdata/email",ui->emailLineEdit->text());

        settings.setValue("flightlogging/function", ui->functionComboBox->currentText());
        settings.setValue("flightlogging/rules", ui->rulesComboBox->currentText());
        settings.setValue("flightlogging/approach", ui->approachComboBox->currentText());
        settings.setValue("flightlogging/nightlogging", ui->functionComboBox->currentIndex());
        settings.setValue("flightlogging/flightnumberPrefix", ui->prefixLineEdit->text());

        QMap<QString,QString> data;
        switch (ui->aliasComboBox->currentIndex()) {
        case 0:
            settings.setValue("userdata/displayselfas","self");
            data.insert("displayname","self");
            break;
        case 1:
            settings.setValue("userdata/displayselfas","SELF");
            data.insert("displayname","SELF");
            break;
        case 2:{
            settings.setValue("userdata/displayselfas","Last,F.");
            QString name;
            name.append(ui->piclastnameLineEdit->text());
            name.append(QLatin1String(", "));
            name.append(ui->picfirstnameLineEdit->text().left(1));
            name.append(QLatin1Char('.'));
            data.insert("displayname",name);
        }
            break;
        default:
            break;
        }
        data.insert("piclastname",ui->piclastnameLineEdit->text());
        data.insert("picfirstname",ui->picfirstnameLineEdit->text());
        data.insert("employeeid",ui->employeeidLineEdit->text());
        data.insert("phone",ui->phoneLineEdit->text());
        data.insert("email",ui->emailLineEdit->text());

        Pilot pic("pilots",1);
        pic.setData(data);
        pic.commit();
        accept();
    }
}
