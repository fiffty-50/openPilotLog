#include "firstrundialog.h"
#include "ui_firstrundialog.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/database/adatabasesetup.h"
#include "src/classes/apilotentry.h"
#include "src/classes/adownload.h"
#include "src/classes/asettings.h"
//const auto TEMPLATE_URL = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/assets/database/templates/");

static inline
void prompt_error_box(QString title, QString text, QWidget* parent = nullptr)
{
    QMessageBox(QMessageBox::Warning, title, text, QMessageBox::Ok, parent).exec();
}

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->lastnameLineEdit->setFocus();
    ui->previousPushButton->setEnabled(false);
    ui->nightComboBox->setCurrentIndex(1);

    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);

    QObject::connect(themeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
                     this, &FirstRunDialog::on_themeGroup_toggled);
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::on_previousPushButton_clicked()
{
    auto current_idx = ui->stackedWidget->currentIndex();
    switch (current_idx) {
    case 0:
        return;
    case 1:
        ui->previousPushButton->setEnabled(false);
        break;
    case 2:
        ui->nextPushButton->setText(QStringLiteral("Next"));
        break;
    }
    ui->stackedWidget->setCurrentIndex(current_idx - 1);

}

void FirstRunDialog::on_nextPushButton_clicked()
{
    auto current_idx = ui->stackedWidget->currentIndex();
    // [G]: per index do appropriate error handling
    switch (current_idx) {
    case 0:
        if(ui->firstnameLineEdit->text().isEmpty()
           || ui->lastnameLineEdit->text().isEmpty())
        {
            prompt_error_box(QStringLiteral("Error"), QStringLiteral("Please enter first and last name"));
            return;
        }
        ui->previousPushButton->setEnabled(true);
        break;
    case 1:
        ui->nextPushButton->setText(QStringLiteral("Done"));
        break;
    case 2:
        finish();
        return;
    }
    ui->stackedWidget->setCurrentIndex(current_idx + 1);
}

void FirstRunDialog::on_themeGroup_toggled(int id)
{
    ASettings::write("main/theme", id);
}

void FirstRunDialog::finish()
{
    ASettings::write("userdata/lastname", ui->lastnameLineEdit->text());
    ASettings::write("userdata/firstname", ui->firstnameLineEdit->text());
    ASettings::write("userdata/employeeid", ui->employeeidLineEdit->text());
    ASettings::write("userdata/phone", ui->phoneLineEdit->text());
    ASettings::write("userdata/email", ui->emailLineEdit->text());

    ASettings::write("flightlogging/function", ui->functionComboBox->currentText());
    ASettings::write("flightlogging/approach", ui->approachComboBox->currentText());
    ASettings::write("flightlogging/nightlogging", ui->nightComboBox->currentIndex());
    ASettings::write("flightlogging/logIfr", ui->rulesComboBox->currentIndex());
    ASettings::write("flightlogging/flightnumberPrefix", ui->prefixLineEdit->text());

    ASettings::write("flightlogging/numberTakeoffs", 1);
    ASettings::write("flightlogging/numberLandings", 1);
    ASettings::write("flightlogging/popupCalendar", true);
    ASettings::write("flightlogging/pilotFlying", true);

    QMap<QString, QVariant> data;
    switch (ui->aliasComboBox->currentIndex()) {
    case 0:
        ASettings::write("userdata/displayselfas", ui->aliasComboBox->currentIndex());
        break;
    case 1:
        ASettings::write("userdata/displayselfas", ui->aliasComboBox->currentIndex());
        break;
    case 2:
        ASettings::write("userdata/displayselfas", ui->aliasComboBox->currentIndex());
        break;
    default:
        break;
    }
    data.insert("lastname", ui->lastnameLineEdit->text());
    data.insert("firstname", ui->firstnameLineEdit->text());
    data.insert("alias", "self");
    data.insert("employeeid", ui->employeeidLineEdit->text());
    data.insert("phone", ui->phoneLineEdit->text());
    data.insert("email", ui->emailLineEdit->text());

    if (!finishSetup()) {
        QMessageBox message_box(this);
        message_box.setText("Errors have ocurred creating the database. Without a working database The application will not be usable.");
    }
    ASettings::write("setup/setup_complete", true);
    aDB()->disconnect(); // reset db connection to refresh layout after initial setup.
    aDB()->connect();
    auto pilot = APilotEntry(1);
    pilot.setData(data);
    if (aDB()->commit(pilot)) {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    } else {
        QMessageBox message_box(this);
        message_box.setText("Errors have ocurred creating the database. Without a working database The application will not be usable.");
    }
}

bool FirstRunDialog::finishSetup()
{
    QMessageBox confirm;
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle("Create Database");
    confirm.setText("We are now going to create the database. Would you like to download the latest database information?\n(Recommended, Internet connection required)\n");

    int reply = confirm.exec();
    if (reply == QMessageBox::Yes)
        ADataBaseSetup::downloadTemplates();

    aDB()->disconnect();
    ADataBaseSetup::backupOldData();
    aDB()->connect();

    if(!ADataBaseSetup::createDatabase())
        return false;
    if(!ADataBaseSetup::importDefaultData())
        return false;
    ASettings::write("setup/setup_complete", true);
    return true;
}


