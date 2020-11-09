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

// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

static const auto FIRSTNAME_VALID = QPair<QString, QRegularExpression> {
    "picfirstnameLineEdit", QRegularExpression("[a-zA-Z]+")};
static const auto LASTNAME_VALID = QPair<QString, QRegularExpression> {
    "piclastnameLineEdit", QRegularExpression("\\w+")};
static const auto PHONE_VALID = QPair<QString, QRegularExpression> {
    "phoneLineEdit", QRegularExpression("^[+]{0,1}[0-9\\-\\s]+")};
static const auto EMAIL_VALID = QPair<QString, QRegularExpression> {
    "emailLineEdit", QRegularExpression("\\A[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"
                                        "(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z")};
static const auto COMPANY_VALID = QPair<QString, QRegularExpression> {
    "companyLineEdit", QRegularExpression("\\w+")};
static const auto EMPLOYEENR_VALID = QPair<QString, QRegularExpression> {
    "employeeidLineEdit", QRegularExpression("\\w+")};
static const auto PREFIX_VALID = QPair<QString, QRegularExpression> {
    "prefixLineEdit", QRegularExpression("[a-zA-Z0-9]?[a-zA-Z0-9]?[a-zA-Z0-9]")};

static const auto LINE_EDIT_VALIDATORS = QVector({FIRSTNAME_VALID, LASTNAME_VALID,
                                           PHONE_VALID,     EMAIL_VALID,
                                           COMPANY_VALID,     EMPLOYEENR_VALID,
                                           PREFIX_VALID});

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);
    connect(themeGroup, SIGNAL(buttonClicked(int)), this, SLOT(themeGroup_toggled(int)));

    fillSettings();
    setupValidators();
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::fillSettings()
{
    /*
     * Personal Tab
     */
    ui->piclastnameLineEdit->setText(Settings::read("userdata/piclastname").toString());
    ui->picfirstnameLineEdit->setText(Settings::read("userdata/picfirstname").toString());
    ui->companyLineEdit->setText(Settings::read("userdata/company").toString());
    ui->employeeidLineEdit->setText(Settings::read("userdata/employeeid").toString());
    ui->phoneLineEdit->setText(Settings::read("userdata/phone").toString());
    ui->emailLineEdit->setText(Settings::read("userdata/email").toString());
    /*
     * Flight Logging Tab
     */
    ui->aliasComboBox->setCurrentIndex(Settings::read("userdata/displayselfas").toInt());
    ui->functionComboBox->setCurrentText(Settings::read("flightlogging/function").toString());
    ui->rulesComboBox->setCurrentText(Settings::read("flightlogging/rules").toString());
    ui->approachComboBox->setCurrentText(Settings::read("flightlogging/approach").toString());
    ui->nightComboBox->setCurrentIndex(Settings::read("flightlogging/nightlogging").toInt());
    ui->prefixLineEdit->setText(Settings::read("flightlogging/flightnumberPrefix").toString());
    /*
     * Misc Tab
     */
    switch (Settings::read("main/theme").toInt()) {
    case 0:
        ui->systemThemeCheckBox->setChecked(true);
        break;
    case 1:
        ui->lightThemeCheckBox->setChecked(true);
        break;
    case 2:
        ui->darkThemeCheckBox->setChecked(true);
    }
    ui->logbookViewComboBox->setCurrentIndex(Settings::read("logbook/view").toInt());
    /*
     * Aircraft Tab
     */
    ui->acSortComboBox->setCurrentIndex(Settings::read("userdata/acSortColumn").toInt());
    ui->pilotSortComboBox->setCurrentIndex(Settings::read("userdata/pilSortColumn").toInt());
    ui->acAllowIncompleteComboBox->setCurrentIndex(Settings::read("userdata/acAllowIncomplete").toInt());
}

void SettingsWidget::setupValidators()
{
    DEB("Setting up Validators...");
    for(const auto& pair : LINE_EDIT_VALIDATORS){
        auto line_edit = parent()->findChild<QLineEdit*>(pair.first);
        if(line_edit != nullptr){
            auto validator = new QRegularExpressionValidator(pair.second,line_edit);
            line_edit->setValidator(validator);
        }else{
            DEB("Error: Line Edit not found: "<< pair.first << " - skipping.");
        }

    }
}

void SettingsWidget::updatePersonalDetails()
{
    QMap<QString,QString> data;
    switch (ui->aliasComboBox->currentIndex()) {
    case 0:
        data.insert("displayname","self");
        break;
    case 1:
        data.insert("displayname","SELF");
        break;
    case 2:{
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
    data.insert("company",ui->companyLineEdit->text());
    data.insert("employeeid",ui->employeeidLineEdit->text());
    data.insert("phone",ui->phoneLineEdit->text());
    data.insert("email",ui->emailLineEdit->text());

    Pilot pic(1);
    pic.setData(data);
    pic.commit();
}

/*
 * Slots
 */


/*
 * Personal Tab
 */

void SettingsWidget::on_piclastnameLineEdit_editingFinished()
{
    if(ui->piclastnameLineEdit->text().isEmpty()){
        ui->piclastnameLineEdit->setText(Settings::read("userdata/piclastname").toString());
        ui->piclastnameLineEdit->setFocus();
    } else {
        Settings::write("userdata/piclastname",ui->piclastnameLineEdit->text());
        updatePersonalDetails();
    }
}

void SettingsWidget::on_picfirstnameLineEdit_editingFinished()
{
    if(ui->picfirstnameLineEdit->text().isEmpty()){
        ui->picfirstnameLineEdit->setText(Settings::read("userdata/picfirstname").toString());
        ui->picfirstnameLineEdit->setFocus();
    } else {
        Settings::write("userdata/picfirstname",ui->picfirstnameLineEdit->text());
        updatePersonalDetails();
    }
}

void SettingsWidget::on_companyLineEdit_editingFinished()
{
    Settings::write("userdata/company",ui->companyLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_employeeidLineEdit_editingFinished()
{
    Settings::write("userdata/employeeid",ui->employeeidLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_emailLineEdit_editingFinished()
{
    Settings::write("userdata/email",ui->emailLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_phoneLineEdit_editingFinished()
{
    Settings::write("userdata/phone",ui->phoneLineEdit->text());
    updatePersonalDetails();
}

/*
 * Flight Logging Tab
 */

void SettingsWidget::on_aliasComboBox_currentIndexChanged(int index)
{
    Settings::write("userdata/displayselfas",index);
    updatePersonalDetails();
}

void SettingsWidget::on_functionComboBox_currentIndexChanged(const QString &arg1)
{
    Settings::write("flightlogging/function", arg1);
}

void SettingsWidget::on_rulesComboBox_currentIndexChanged(const QString &arg1)
{
    Settings::write("flightlogging/rules", arg1);
}

void SettingsWidget::on_approachComboBox_currentIndexChanged(const QString &arg1)
{
    Settings::write("flightlogging/approach", arg1);
}

void SettingsWidget::on_nightComboBox_currentIndexChanged(int index)
{
    Settings::write("flightlogging/nightlogging", index);
    switch (index) {
    case 1:
        Settings::write("flightlogging/nightangle",-6);
        break;
    case 2:
        Settings::write("flightlogging/nightangle",0);
        break;
    default:
        Settings::write("flightlogging/nightangle",-6);
    }
}

void SettingsWidget::on_prefixLineEdit_textChanged(const QString &arg1)
{
    Settings::write("flightlogging/flightnumberPrefix", arg1);
}

/*
 * Misc Tab
 */
void SettingsWidget::themeGroup_toggled(int id)
{
    Settings::write("main/theme", id);

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Changing Themes",
                                  "Changing the theme requires restarting the Application.\n\nWould you like to restart now?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());

    } else {
        QMessageBox *info = new QMessageBox(this);
        info->setText("Theme change will take effect the next time you start the application.");
        info->exec();
    }
}
void SettingsWidget::on_logbookViewComboBox_currentIndexChanged(int index)
{
    Settings::write("logbook/view", index);
}
void SettingsWidget::on_pilotSortComboBox_currentIndexChanged(int index)
{
    Settings::write("userdata/pilSortColumn", index);
}

void SettingsWidget::on_acSortComboBox_currentIndexChanged(int index)
{
    Settings::write("userdata/acSortColumn", index);
}

void SettingsWidget::on_acAllowIncompleteComboBox_currentIndexChanged(int index)
{
    Settings::write("userdata/acAllowIncomplete", index);
    if (index) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Warning",
                                      "Enabling incomplete logging will enable you to add aircraft with incomplete data.\n\n"
                                      "If you do not fill out the aircraft details, "
                                      "it will be impossible to automatically determine Single/Multi Pilot Times or Single/Multi Engine Time. "
                                      "This will also impact statistics and auto-logging capabilites as well as jeopardise database integrity.\n\n"
                                      "It is highly recommended to keep this option off unless you have a specific reason not to.\n\n"
                                      "Are you sure you want to proceed?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            Settings::write("userdata/acAllowIncomplete", index);
        } else {
            ui->acAllowIncompleteComboBox->setCurrentIndex(0);
        }
    }
}

/*
 * About Tab
 */


void SettingsWidget::on_aboutPushButton_clicked()
{
    auto mb = new QMessageBox(this);
    QString SQLITE_VERSION = DbInfo().version;
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
                       "<a href=\"https://sqlite.org/about.html\">SQLite</a> version %4</p>"
                   ).arg(QLatin1String("github.com/fiffty-50/openpilotlog"),
                         QLatin1String("qt.io"),
                         QLatin1String(QT_VERSION_STR),
                         QString(SQLITE_VERSION));
    mb->setText(text);
    mb->open();
}
