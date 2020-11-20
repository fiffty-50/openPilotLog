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
    if(ui->tabWidget->currentIndex()<2){
        ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()+1);
    } else {
        emit ui->finishButton->clicked();
    }

}

void FirstRunDialog::themeGroup_toggled(int id)
{
    Settings::write("main/theme", id);
}

void FirstRunDialog::on_finishButton_clicked()
{
    if(ui->piclastnameLineEdit->text().isEmpty() || ui->picfirstnameLineEdit->text().isEmpty()){
        auto mb = new QMessageBox(this);
        mb->setText("You have to enter a valid first and last name for the logbook.");
        mb->show();
    } else {
        Settings::write("userdata/piclastname",ui->piclastnameLineEdit->text());
        Settings::write("userdata/picfirstname",ui->picfirstnameLineEdit->text());
        Settings::write("userdata/employeeid",ui->employeeidLineEdit->text());
        Settings::write("userdata/phone",ui->phoneLineEdit->text());
        Settings::write("userdata/email",ui->emailLineEdit->text());

        Settings::write("flightlogging/function", ui->functionComboBox->currentText());
        Settings::write("flightlogging/rules", ui->rulesComboBox->currentText());
        Settings::write("flightlogging/approach", ui->approachComboBox->currentText());
        Settings::write("flightlogging/nightlogging", ui->nightComboBox->currentIndex());
        Settings::write("flightlogging/flightnumberPrefix", ui->prefixLineEdit->text());

        QMap<QString,QString> data;
        switch (ui->aliasComboBox->currentIndex()) {
        case 0:
            Settings::write("userdata/displayselfas",ui->aliasComboBox->currentIndex());
            data.insert("displayname","self");
            break;
        case 1:
            Settings::write("userdata/displayselfas",ui->aliasComboBox->currentIndex());
            data.insert("displayname","SELF");
            break;
        case 2:{
            Settings::write("userdata/displayselfas",ui->aliasComboBox->currentIndex());
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

        Pilot pic(1);
        pic.setData(data);
        pic.commit();

        switch(Settings::read("main/theme").toInt()){
        case 0:
            accept();
            break;
        case 1:
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
            break;
        case 2:
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
            break;
        default:
            Settings::write("main/theme", 0);
            accept();
        }
    }
}
