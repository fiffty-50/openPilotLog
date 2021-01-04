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
#include "src/testing/adebug.h"
#include "src/classes/astyle.h"
#include "src/classes/asettings.h"
#include "src/database/adatabase.h"
#include "src/classes/apilotentry.h"

#include <QStyleFactory>

static const auto FIRSTNAME_VALID = QPair<QString, QRegularExpression> {
    "firstnameLineEdit", QRegularExpression("[a-zA-Z]+")};
static const auto LASTNAME_VALID = QPair<QString, QRegularExpression> {
    "lastnameLineEdit", QRegularExpression("\\w+")};
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

    auto styles = AStyle::styles;
    auto current_style = AStyle::style();
    ui->styleComboBox->addItem(current_style);
    styles.removeOne(current_style);

    ui->styleComboBox->addItems(styles);
    ui->styleComboBox->model()->sort(0);
    ui->styleComboBox->setCurrentText(current_style);

    if(ASettings::read(ASettings::Main::StyleSheet).toUInt() == AStyle::Dark)
        ui->darkStyleCheckBox->setCheckState(Qt::Checked);

    readSettings();
    setupValidators();
}


SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::readSettings()
{
    /*
     * Personal Tab
     */
    ui->lastnameLineEdit->setText(ASettings::read(ASettings::UserData::LastName).toString());
    ui->firstnameLineEdit->setText(ASettings::read(ASettings::UserData::FirstName).toString());
    ui->companyLineEdit->setText(ASettings::read(ASettings::UserData::Company).toString());
    ui->employeeidLineEdit->setText(ASettings::read(ASettings::UserData::EmployeeID).toString());
    ui->phoneLineEdit->setText(ASettings::read(ASettings::UserData::Phone).toString());
    ui->emailLineEdit->setText(ASettings::read(ASettings::UserData::Email).toString());
    /*
     * Flight Logging Tab
     */
    ui->aliasComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::DisplaySelfAs).toInt());
    ui->functionComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Function).toString());
    ui->rulesComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Rules).toString());
    ui->approachComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Approach).toString());
    ui->nightComboBox->setCurrentIndex(ASettings::read(ASettings::FlightLogging::NightLogging).toInt());
    ui->prefixLineEdit->setText(ASettings::read(ASettings::FlightLogging::FlightNumberPrefix).toString());

    ui->logbookViewComboBox->setCurrentIndex(ASettings::read(ASettings::LogBook::View).toInt());
    /*
     * Aircraft Tab
     */
    ui->acSortComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::AcSortColumn).toInt());
    ui->pilotSortComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::PilSortColumn).toInt());
    ui->acAllowIncompleteComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::AcAllowIncomplete).toInt());
}

void SettingsWidget::setupValidators()
{
    DEB << "Setting up Validators...";
    for(const auto& pair : LINE_EDIT_VALIDATORS){
        auto line_edit = parent()->findChild<QLineEdit*>(pair.first);
        if(line_edit != nullptr){
            auto validator = new QRegularExpressionValidator(pair.second,line_edit);
            line_edit->setValidator(validator);
        }else{
            DEB << "Error: Line Edit not found: "<< pair.first << " - skipping.";
        }

    }
}

void SettingsWidget::updatePersonalDetails()
{
    QMap<QString, QVariant> data;
    switch (ui->aliasComboBox->currentIndex()) {
    case 0:
        data.insert("alias", "self");
        break;
    case 1:
        data.insert("alias","SELF");
        break;
    case 2:{
        QString name;
        name.append(ui->lastnameLineEdit->text());
        name.append(QLatin1String(", "));
        name.append(ui->firstnameLineEdit->text().left(1));
        name.append(QLatin1Char('.'));
        data.insert("alias", name);
    }
        break;
    default:
        break;
    }
    data.insert("lastname", ui->lastnameLineEdit->text());
    data.insert("firstname", ui->firstnameLineEdit->text());
    data.insert("company", ui->companyLineEdit->text());
    data.insert("employeeid", ui->employeeidLineEdit->text());
    data.insert("phone", ui->phoneLineEdit->text());
    data.insert("email", ui->emailLineEdit->text());

    auto pic = APilotEntry(1);
    pic.setData(data);

    aDB()->commit(pic);
}

/*
 * Slots
 */


/*
 * Personal Tab
 */

void SettingsWidget::on_lastnameLineEdit_editingFinished()
{
    if(ui->lastnameLineEdit->text().isEmpty()){
        ui->lastnameLineEdit->setText(ASettings::read(ASettings::UserData::LastName).toString());
        ui->lastnameLineEdit->setFocus();
    } else {
        ASettings::write(ASettings::UserData::LastName, ui->lastnameLineEdit->text());
        updatePersonalDetails();
    }
}

void SettingsWidget::on_firstnameLineEdit_editingFinished()
{
    if(ui->firstnameLineEdit->text().isEmpty()){
        ui->firstnameLineEdit->setText(ASettings::read(ASettings::UserData::FirstName).toString());
        ui->firstnameLineEdit->setFocus();
    } else {
        ASettings::write(ASettings::UserData::FirstName,ui->firstnameLineEdit->text());
        updatePersonalDetails();
    }
}

void SettingsWidget::on_companyLineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Company, ui->companyLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_employeeidLineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::EmployeeID, ui->employeeidLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_emailLineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Email, ui->emailLineEdit->text());
    updatePersonalDetails();
}

void SettingsWidget::on_phoneLineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Phone, ui->phoneLineEdit->text());
    updatePersonalDetails();
}

/*
 * Flight Logging Tab
 */

void SettingsWidget::on_aliasComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::DisplaySelfAs, index);
    updatePersonalDetails();
}

void SettingsWidget::on_functionComboBox_currentIndexChanged(const QString &arg1)
{
    ASettings::write(ASettings::FlightLogging::Function, arg1);
}

void SettingsWidget::on_rulesComboBox_currentIndexChanged(const QString &arg1)
{
    ASettings::write(ASettings::FlightLogging::Rules, arg1);
}

void SettingsWidget::on_approachComboBox_currentIndexChanged(const QString &arg1)
{
    ASettings::write(ASettings::FlightLogging::Approach, arg1);
}

void SettingsWidget::on_nightComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::FlightLogging::NightLogging, index);
    switch (index) {
    case 1:
        ASettings::write(ASettings::FlightLogging::NightAngle, -6);
        break;
    case 2:
        ASettings::write(ASettings::FlightLogging::NightAngle, 0);
        break;
    default:
        ASettings::write(ASettings::FlightLogging::NightAngle, -6);
    }
}

void SettingsWidget::on_prefixLineEdit_textChanged(const QString &arg1)
{
    ASettings::write(ASettings::FlightLogging::FlightNumberPrefix, arg1);

}

/*
 * Misc Tab
 */

void SettingsWidget::on_logbookViewComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::LogBook::View, index);
    emit viewSelectionChanged(index);
}
void SettingsWidget::on_pilotSortComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::PilSortColumn, index);
}

void SettingsWidget::on_acSortComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::AcSortColumn, index);
}

void SettingsWidget::on_acAllowIncompleteComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::AcAllowIncomplete, index);
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
            ASettings::write(ASettings::UserData::AcAllowIncomplete, index);
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
    auto message_box = QMessageBox(this);
    QString SQLITE_VERSION = aDB()->sqliteVersion();
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
    message_box.setText(text);
    message_box.exec();
}

void SettingsWidget::on_styleComboBox_currentTextChanged(const QString& text)
{
    DEB << text;
    AStyle::setStyle(text);
}

void SettingsWidget::on_darkStyleCheckBox_stateChanged(int state)
{
    DEB << "Setting to:" << (state ? "dark" : "default");
    if(state == Qt::Checked)
        AStyle::setStyleSheet(AStyle::Dark);
    else
        AStyle::setStyleSheet(AStyle::Default);
}
