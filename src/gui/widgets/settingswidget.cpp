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
    QSettings settings;
    /*
     * Personal Tab
     */
    ui->piclastnameLineEdit->setText(settings.value("userdata/piclastname").toString());
    ui->picfirstnameLineEdit->setText(settings.value("userdata/picfirstname").toString());
    ui->employeeidLineEdit->setText(settings.value("userdata/employeeid").toString());
    ui->phoneLineEdit->setText(settings.value("userdata/phone").toString());
    ui->emailLineEdit->setText(settings.value("userdata/email").toString());
    /*
     * Flight Logging Tab
     */
    ui->aliasComboBox->setCurrentIndex(settings.value("userdata/displayselfas").toInt());
    ui->functionComboBox->setCurrentText(settings.value("flightlogging/function").toString());
    ui->rulesComboBox->setCurrentText(settings.value("flightlogging/rules").toString());
    ui->approachComboBox->setCurrentText(settings.value("flightlogging/approach").toString());
    ui->nightComboBox->setCurrentIndex(settings.value("flightlogging/nightlogging").toInt());
    ui->prefixLineEdit->setText(settings.value("flightlogging/flightnumberPrefix").toString());
    /*
     * Theme Group
     */
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
     * Aircraft Tab
     */
    ui->acSortComboBox->setCurrentIndex(settings.value("userdata/acSortColumn").toInt());
    ui->pilotSortComboBox->setCurrentIndex(settings.value("userdata/pilSortColumn").toInt());
    ui->acAllowIncompleteComboBox->setCurrentIndex(settings.value("userdata/acAllowIncomplete").toInt());
}

void SettingsWidget::setupValidators()
{
    DEB("Setting up Validators...");
    for(const auto& pair : LINE_EDIT_VALIDATORS){
        auto line_edit = parent()->findChild<QLineEdit*>(pair.first);
        auto validator = new QRegularExpressionValidator(pair.second,line_edit);
        line_edit->setValidator(validator);
    }
}

void SettingsWidget::updatePersonalDetails()
{
    QSettings settings;
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
    QSettings settings;
    if(ui->piclastnameLineEdit->text().isEmpty()){
        ui->piclastnameLineEdit->setText(settings.value("userdata/piclastname").toString());
        ui->piclastnameLineEdit->setFocus();
    } else {
        settings.setValue("userdata/piclastname",ui->piclastnameLineEdit->text());
        updatePersonalDetails();
    }
}

void SettingsWidget::on_picfirstnameLineEdit_editingFinished()
{
    QSettings settings;
    if(ui->picfirstnameLineEdit->text().isEmpty()){
        ui->picfirstnameLineEdit->setText(settings.value("userdata/picfirstname").toString());
        ui->picfirstnameLineEdit->setFocus();
    } else {
        settings.setValue("userdata/picfirstname",ui->picfirstnameLineEdit->text());
        updatePersonalDetails();
    }
}

void SettingsWidget::on_companyLineEdit_editingFinished()
{
    QSettings settings;
    settings.setValue("userdata/company",ui->companyLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_employeeidLineEdit_editingFinished()
{
    QSettings settings;
    settings.setValue("userdata/employeeid",ui->employeeidLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_emailLineEdit_editingFinished()
{
    QSettings settings;
    settings.setValue("userdata/email",ui->emailLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_phoneLineEdit_editingFinished()
{
    QSettings settings;
    settings.setValue("userdata/phone",ui->phoneLineEdit->text());
    updatePersonalDetails();
}

/*
 * Flight Logging Tab
 */

void SettingsWidget::on_aliasComboBox_currentIndexChanged(int index)
{
    QSettings settings;
    settings.setValue("userdata/displayselfas",index);
    updatePersonalDetails();
}

void SettingsWidget::on_functionComboBox_currentIndexChanged(const QString &arg1)
{
    QSettings settings;
    settings.setValue("flightlogging/function", arg1);
}

void SettingsWidget::on_rulesComboBox_currentIndexChanged(const QString &arg1)
{
    QSettings settings;
    settings.setValue("flightlogging/rules", arg1);
}

void SettingsWidget::on_approachComboBox_currentIndexChanged(const QString &arg1)
{
    QSettings settings;
    settings.setValue("flightlogging/approach", arg1);
}

void SettingsWidget::on_nightComboBox_currentIndexChanged(int index)
{
    if(index == 2){
        auto mb = new QMessageBox(this);
        mb->setText("This option is not yet available.");
        mb->show();
        ui->nightComboBox->setCurrentIndex(0);
    } else {
        QSettings settings;
        settings.setValue("flightlogging/nightlogging", index);
    }

}

void SettingsWidget::on_prefixLineEdit_textChanged(const QString &arg1)
{
    QSettings settings;
    settings.setValue("flightlogging/flightnumberPrefix", arg1);
}

/*
 * Misc Tab
 */
void SettingsWidget::themeGroup_toggled(int id)
{
    QSettings settings;
    settings.setValue("main/theme", id);


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
    QSettings settings;
    settings.setValue("logbook/view", index);
}
void SettingsWidget::on_pilotSortComboBox_currentIndexChanged(int index)
{
    QSettings settings;
    settings.setValue("userdata/pilSortColumn", index);
}

void SettingsWidget::on_acSortComboBox_currentIndexChanged(int index)
{
    QSettings settings;
    settings.setValue("userdata/acSortColumn", index);
}

void SettingsWidget::on_acAllowIncompleteComboBox_currentIndexChanged(int index)
{
    QSettings settings;
    settings.setValue("userdata/acAllowIncomplete", index);
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
            QSettings settings;
            settings.setValue("userdata/acAllowIncomplete", index);
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
