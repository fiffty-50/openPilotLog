/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "src/gui/dialogues/exporttocsvdialog.h"
#include "src/gui/widgets/totalswidget.h"
#include "ui_settingswidget.h"
#include "src/classes/style.h"
#include "src/classes/settings.h"
#include "src/database/database.h"
#include "src/database/row.h"
#include "src/opl.h"
#include "src/functions/datetime.h"
#include "src/gui/widgets/backupwidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    loadBackupWidget();
    loadPreviousExperienceWidget();
    setupComboBoxes();
    setupDateEdits();
    setupValidators();
    readSettings();
}


SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
}

void SettingsWidget::setupComboBoxes(){
    {
        // Set up Combo Boxes
        OPL::Style::loadStylesComboBox(ui->styleComboBox);
        OPL::GLOBALS->loadApproachTypes(ui->approachComboBox);
        OPL::GLOBALS->loadPilotFunctios(ui->functionComboBox);
        OPL::GLOBALS->fillViewNamesComboBox(ui->logbookViewComboBox);
        OPL::GLOBALS->fillLanguageComboBox(ui->languageComboBox);
    }
}


void SettingsWidget::setupDateEdits()
{
    // Read Display Format Setting
    int date_format_index = Settings::read(Settings::Main::DateFormat).toInt();
    const QString date_format_string = OPL::DateTime::getFormatString(
                static_cast<OPL::DateFormat>(date_format_index));
    const auto date_edits = this->findChildren<QDateEdit*>();
    for (const auto &date_edit : date_edits) {
        date_edit->setDisplayFormat(date_format_string);
    }
    // Fill currencies
    const QList<QPair<OPL::CurrencyName, QDateEdit*>> currencies_list = {
        {OPL::CurrencyName::Licence,    ui->currLicDateEdit},
        {OPL::CurrencyName::TypeRating, ui->currTrDateEdit},
        {OPL::CurrencyName::LineCheck,  ui->currLckDateEdit},
        {OPL::CurrencyName::Medical,    ui->currMedDateEdit},
        {OPL::CurrencyName::Custom1,    ui->currCustom1DateEdit},
        {OPL::CurrencyName::Custom2,    ui->currCustom2DateEdit},
    };
    for (const auto &pair : currencies_list) {
        const QSignalBlocker signal_blocker(pair.second);
        const auto entry = DB->getCurrencyEntry(static_cast<int>(pair.first));
        if (entry.isValid()) { // set date
            const auto date = QDate::fromString(
                        entry.getData().value(OPL::Db::CURRENCIES_EXPIRYDATE).toString(),
                        Qt::ISODate);
            if(date.isValid())
                pair.second->setDate(date);
        } else { // set current date
            pair.second->setDate(QDate::currentDate());
        }
    }
}

void SettingsWidget::loadBackupWidget()
{
    auto bw = new BackupWidget(this);
    ui->backupStackedWidget->addWidget(bw);
    ui->backupStackedWidget->setCurrentWidget(bw);
}

void SettingsWidget::loadPreviousExperienceWidget()
{
    auto pxp = new TotalsWidget(TotalsWidget::WidgetType::PreviousExperienceWidget);
    ui->previousExpStackedWidget->addWidget(pxp);
    ui->previousExpStackedWidget->setCurrentWidget(pxp);
}

/*!
 * \brief SettingsWidget::readSettings Reads settings from Settings and sets up the UI accordingly
 */
void SettingsWidget::readSettings()
{
    //const QSignalBlocker blocker(this); // don't emit editing finished for setting these values

    // Personal Data Tab
    auto user_data = DB->getPilotEntry(1).getData();
    ui->lastnameLineEdit->setText(user_data.value(OPL::Db::PILOTS_LASTNAME).toString());
    ui->firstnameLineEdit->setText(user_data.value(OPL::Db::PILOTS_FIRSTNAME).toString());
    ui->companyLineEdit->setText(user_data.value(OPL::Db::PILOTS_COMPANY).toString());
    ui->employeeidLineEdit->setText(user_data.value(OPL::Db::PILOTS_EMPLOYEEID).toString());
    ui->phoneLineEdit->setText(user_data.value(OPL::Db::PILOTS_PHONE).toString());
    ui->emailLineEdit->setText(user_data.value(OPL::Db::PILOTS_EMAIL).toString());

    // FLight Logging Tab
    ui->functionComboBox->setCurrentIndex(Settings::read(Settings::FlightLogging::Function).toInt());
    ui->rulesComboBox->setCurrentIndex(Settings::read(Settings::FlightLogging::LogIFR).toInt());
    ui->approachComboBox->setCurrentIndex(Settings::read(Settings::FlightLogging::Approach).toInt());
    ui->nightComboBox->setCurrentIndex(Settings::read(Settings::FlightLogging::NightLoggingEnabled).toInt());
    ui->prefixLineEdit->setText(Settings::read(Settings::FlightLogging::FlightNumberPrefix).toString());

    ui->logbookViewComboBox->setCurrentIndex(Settings::read(Settings::Main::LogbookView).toInt());
    ui->aliasComboBox->setCurrentIndex(Settings::read(Settings::UserData::DisplaySelfAs).toInt());

    // Currencies Tab
    ui->currToLdgCheckBox->setChecked(Settings::read(Settings::UserData::ShowToLgdCurrency).toBool());
    ui->currLicCheckBox->setChecked(Settings::read(Settings::UserData::ShowLicCurrency).toBool());
    ui->currTrCheckBox->setChecked(Settings::read(Settings::UserData::ShowTrCurrency).toBool());
    ui->currLckCheckBox->setChecked(Settings::read(Settings::UserData::ShowLckCurrency).toBool());
    ui->currMedCheckBox->setChecked(Settings::read(Settings::UserData::ShowMedCurrency).toBool());
    ui->currCustom1CheckBox->setChecked(Settings::read(Settings::UserData::ShowCustom1Currency).toBool());
    ui->currCustom2CheckBox->setChecked(Settings::read(Settings::UserData::ShowCustom2Currency).toBool());
    ui->currCustom1LineEdit->setText(Settings::read(Settings::UserData::Custom1CurrencyName).toString());
    ui->currCustom2LineEdit->setText(Settings::read(Settings::UserData::Custom2CurrencyName).toString());

    // Misc Tab
    ui->acftSortComboBox->setCurrentIndex(Settings::read(Settings::UserData::TailSortColumn).toInt());
    ui->pilotSortComboBox->setCurrentIndex(Settings::read(Settings::UserData::PilotSortColumn).toInt());

    // Don't emit signals for OPL::Style changes during setup
    const QSignalBlocker style_blocker(ui->styleComboBox);
    const QSignalBlocker font_blocker_1(ui->fontSpinBox);
    const QSignalBlocker font_blocker_2(ui->fontComboBox);
    const QSignalBlocker font_blocker_3(ui->fontCheckBox);

    ui->styleComboBox->setCurrentText(Settings::read(Settings::Main::Style).toString());
    ui->fontSpinBox->setValue(Settings::read(Settings::Main::FontSize).toUInt());
    ui->fontComboBox->setCurrentFont(QFont(Settings::read(Settings::Main::Font).toString()));
    bool use_system_font = Settings::read(Settings::Main::UseSystemFont).toBool();
    ui->fontCheckBox->setChecked(use_system_font);
    if (!use_system_font) {
        ui->fontComboBox->setEnabled(true);
        ui->fontSpinBox->setEnabled(true);
    }
}

void SettingsWidget::setupValidators()
{
    ui->phoneLineEdit->setValidator(new QRegularExpressionValidator(OPL::RegEx::RX_PHONE_NUMBER, ui->phoneLineEdit));
    ui->emailLineEdit->setValidator(new QRegularExpressionValidator(OPL::RegEx::RX_EMAIL_ADDRESS, ui->emailLineEdit));
}

/*!
 * \brief SettingsWidget::updatePersonalDetails Updates the database with the users personal details.
 */
void SettingsWidget::updatePersonalDetails()
{
    OPL::RowData_T user_data;
    switch (ui->aliasComboBox->currentIndex()) {
    case 0:
        user_data.insert(OPL::Db::PILOTS_ALIAS, QStringLiteral("self"));
        break;
    case 1:
        user_data.insert(OPL::Db::PILOTS_ALIAS, QStringLiteral("SELF"));
        break;
    case 2:{
        QString name;
        name.append(ui->lastnameLineEdit->text());
        name.append(QLatin1String(", "));
        name.append(ui->firstnameLineEdit->text().at(0));
        name.append(QLatin1Char('.'));
        user_data.insert(OPL::Db::PILOTS_ALIAS, name);
    }
        break;
    default:
        break;
    }
    user_data.insert(OPL::Db::PILOTS_LASTNAME, ui->lastnameLineEdit->text());
    user_data.insert(OPL::Db::PILOTS_FIRSTNAME, ui->firstnameLineEdit->text());
    user_data.insert(OPL::Db::PILOTS_COMPANY, ui->companyLineEdit->text());
    user_data.insert(OPL::Db::PILOTS_EMPLOYEEID, ui->employeeidLineEdit->text());
    user_data.insert(OPL::Db::PILOTS_PHONE, ui->phoneLineEdit->text());
    user_data.insert(OPL::Db::PILOTS_EMAIL, ui->emailLineEdit->text());

    auto user = OPL::PilotEntry(1, user_data);

    TODO << "Changing DB does not currently refresh logbook view";
    TODO << "Check for empty line edits (First, last name should not be empty...validators not a good way because it gives no user feedback)";

    if(!DB->commit(user))
        WARN(tr("Unable to update Database:<br>") + DB->lastError.text());
    else
        LOG << "User updated successfully.";


}

/*
 * Personal Tab
 */

void SettingsWidget::on_lastnameLineEdit_editingFinished()
{
    updatePersonalDetails();
}

void SettingsWidget::on_firstnameLineEdit_editingFinished()
{
    updatePersonalDetails();
}

void SettingsWidget::on_companyLineEdit_editingFinished()
{
    updatePersonalDetails();
}

void SettingsWidget::on_employeeidLineEdit_editingFinished()
{
    updatePersonalDetails();
}

void SettingsWidget::on_emailLineEdit_editingFinished()
{
    updatePersonalDetails();
}

void SettingsWidget::on_phoneLineEdit_editingFinished()
{
    updatePersonalDetails();
}

/*
 * Flight Logging Tab
 */

void SettingsWidget::on_aliasComboBox_currentIndexChanged(int index)
{
    Settings::write(Settings::UserData::DisplaySelfAs, index);
    updatePersonalDetails();
}

void SettingsWidget::on_functionComboBox_currentIndexChanged(int arg1)
{
    Settings::write(Settings::FlightLogging::Function, arg1);
}

void SettingsWidget::on_rulesComboBox_currentIndexChanged(int arg1)
{
    Settings::write(Settings::FlightLogging::LogIFR, arg1);
}

void SettingsWidget::on_approachComboBox_currentIndexChanged(int arg1)
{
    Settings::write(Settings::FlightLogging::Approach, arg1);
}

void SettingsWidget::on_nightComboBox_currentIndexChanged(int index)
{
    Settings::write(Settings::FlightLogging::NightLoggingEnabled, index);
    switch (index) {
    case 1:
        Settings::write(Settings::FlightLogging::NightAngle, -6);
        break;
    case 2:
        Settings::write(Settings::FlightLogging::NightAngle, 0);
        break;
    default:
        Settings::write(Settings::FlightLogging::NightAngle, -6);
    }
}

void SettingsWidget::on_prefixLineEdit_textChanged(const QString &arg1)
{
    Settings::write(Settings::FlightLogging::FlightNumberPrefix, arg1);

}

/*
 * Misc Tab
 */

void SettingsWidget::on_logbookViewComboBox_currentIndexChanged(int index)
{
    Settings::write(Settings::Main::LogbookView, index);
    emit settingChanged(SettingSignal::LogbookWidget);
}
void SettingsWidget::on_pilotSortComboBox_currentIndexChanged(int index)
{
    Settings::write(Settings::UserData::PilotSortColumn, index);
    emit settingChanged(PilotsWidget);
}

void SettingsWidget::on_acftSortComboBox_currentIndexChanged(int index)
{
    Settings::write(Settings::UserData::TailSortColumn, index);
    emit settingChanged(AircraftWidget);
}

/*
 * About Tab
 */

/*!
 * \brief SettingsWidget::on_aboutPushButton_clicked Displays Application Version and Licensing information
 */
void SettingsWidget::on_aboutPushButton_clicked()
{
    QMessageBox message_box(this);
    QPixmap icon = QPixmap(OPL::Assets::ICON_MAIN);
    message_box.setIconPixmap(icon.scaledToWidth(64, Qt::TransformationMode::SmoothTransformation));
    QString SQLITE_VERSION = DB->sqliteVersion();
    QString text = QMessageBox::tr(

                       "<h3><center>About</center></h3>"
                       "<br>"
                       "&#169; 2020 - 2022 Felix Turowsky"
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
                       "please click <a href=\"https://%2\">here</a>.</p>"

                       "<br>"
                       "You are using openPilotLog version %3."
                       "<br>"

                       "<p>This program uses <a href=\"http://%4/\">Qt</a> version %5 and "
                       "<a href=\"https://%6/\">SQLite</a> version %7</p>"
                   ).arg(
                         QStringLiteral("github.com/fiffty-50/openpilotlog"),
                         QStringLiteral("gnu.org/licenses/"),
                         OPL_VERSION_STRING,
                         QStringLiteral("qt.io"),
                         QT_VERSION_STR,
                         QStringLiteral("sqlite.org/about.html"),
                         SQLITE_VERSION);
    message_box.setText(text);
    message_box.exec();
}

void SettingsWidget::on_aboutBackupsPushButton_clicked()
{
    QString text = tr(

                      "<h3><center>About Backups</center></h3>"
                      "<br>"
                      "<p>By creating a backup, you create a copy of your logbook for safekeeping. This copy includes all your "
                      "flights, pilots, aircraft and currencies. By creating a backup, you are creating a snapshot of your logbook to date. This backup can "
                      "later be restored. OpenPilotLog offers two kinds of backups: Local and External Backups.<br><br>Local backups "
                      "are automatically stored in a folder on this computer and will show up in the list below. They can easily be created by selecting <b>Create Local backup</b> and restored with "
                      "<b>Restore Local Backup</b>.<br><br>"
                      "When using <b>Create External Backup</b>, you will be asked where to save your backup file. This can be an external hard drive, USB stick, a cloud location or any other location of your choice. "
                      "This functionality can also be used to sync your database across devices or to take it with you when you buy a new PC. You can then import your backup file by selecting "
                      "it with <b>Restore external backup</b>.</p>"
                      "<p>Frequent backups are recommended to prevent data loss or corruption. It is also recommended to keep a backup copy in a location physically seperated from your main "
                      "computer to prevent data loss due to system failures.</p>"
                      //todo "<p>By default, OpenPilotLog creates a weekly automatic backup. If you would like to change this behaviour, you can adjust it in the settings.</p>"
                      "<br>"
                      );
    QMessageBox msg_box(QMessageBox::Information, "About backups", text, QMessageBox::Ok, this);
    msg_box.exec();
}

void SettingsWidget::on_styleComboBox_currentTextChanged(const QString& new_style_setting)
{
    if (new_style_setting == QLatin1String("Dark-Palette")) {
        OPL::Style::setStyle(OPL::Style::darkPalette());
        Settings::write(Settings::Main::Style, new_style_setting);
        emit settingChanged(MainWindow);
        return;
    }
    for (const auto &style_name : OPL::Style::styles) {
        if (new_style_setting == style_name) {
            OPL::Style::setStyle(style_name);
            Settings::write(Settings::Main::Style, new_style_setting);
            emit settingChanged(MainWindow);
            return;
        }
    }

    for (const auto &style_sheet : OPL::Style::styleSheets) {
        if (new_style_setting == style_sheet.styleSheetName) {
            OPL::Style::setStyle(style_sheet);
            Settings::write(Settings::Main::Style, new_style_setting);
            emit settingChanged(MainWindow);
            return;
        }
    }
}

void SettingsWidget::on_fontComboBox_currentFontChanged(const QFont &f)
{
    qApp->setFont(f);
    Settings::write(Settings::Main::Font, f.toString());
    DEB << "Setting Font:" << f.toString();
}

void SettingsWidget::on_fontSpinBox_valueChanged(int arg1)
{
    QFont f = qApp->font();
    f.setPointSize(arg1);
    qApp->setFont(f);
    Settings::write(Settings::Main::FontSize, arg1);
    DEB << "Setting Font:" << f.toString();
}

void SettingsWidget::on_fontCheckBox_stateChanged(int arg1)
{
    if (usingStylesheet() && arg1 == Qt::Unchecked) {
        WARN(tr("The OPL::Style you have currently selected may not be fully compatible "
                "with changing to a custom font while the application is running.<br><br>"
                "Applying your changes may require restarting the application.<br>"));
    }
    switch (arg1) {
    case Qt::Unchecked:
    {
        ui->fontComboBox->setEnabled(true);
        ui->fontSpinBox->setEnabled(true);
        Settings::write(Settings::Main::UseSystemFont, false);
        QFont font(ui->fontComboBox->currentFont());
        font.setPointSize(ui->fontSpinBox->value());
        qApp->setFont(font);
        LOG << "Setting Font:" << font.toString();
        break;
    }
    case Qt::Checked:
    {
        ui->fontComboBox->setEnabled(false);
        ui->fontSpinBox->setEnabled(false);
        Settings::write(Settings::Main::UseSystemFont, true);
        INFO(tr("The application will be restarted for this change to take effect."));
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
        break;
    default:
        break;
    }
}

/*!
 * \brief Determines if the user has selected a OPL::Stylesheet or is using a Qt OPL::Style Factory Style
 */
bool SettingsWidget::usingStylesheet()
{
    for (const auto &style_sheet : OPL::Style::styleSheets) {
        if (style_sheet.styleSheetName == ui->styleComboBox->currentText())
            return true;
    }
    return false;
}

void SettingsWidget::on_resetStylePushButton_clicked()
{
    LOG << "Resetting OPL::Style to default...";
    ui->styleComboBox->setCurrentText(OPL::Style::defaultStyle);
    ui->fontCheckBox->setChecked(true);
}

void SettingsWidget::on_currLicDateEdit_userDateChanged(const QDate &date)
{
    const OPL::RowData_T row_data = {{OPL::Db::CURRENCIES_EXPIRYDATE, date.toString(Qt::ISODate)}};
    const OPL::CurrencyEntry entry(static_cast<int>(OPL::CurrencyName::Licence), row_data);
    if (!DB->commit(entry))
        WARN(tr("Unable to update currency. The following error has ocurred:<br>%1").arg(DB->lastError.text()));

    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currTrDateEdit_userDateChanged(const QDate &date)
{
    const OPL::RowData_T row_data = {{OPL::Db::CURRENCIES_EXPIRYDATE, date.toString(Qt::ISODate)}};
    const OPL::CurrencyEntry entry(static_cast<int>(OPL::CurrencyName::TypeRating), row_data);
    if (!DB->commit(entry))
        WARN(tr("Unable to update currency. The following error has ocurred:<br>%1").arg(DB->lastError.text()));

    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currLckDateEdit_userDateChanged(const QDate &date)
{
    const OPL::RowData_T row_data = {{OPL::Db::CURRENCIES_EXPIRYDATE, date.toString(Qt::ISODate)}};
    const OPL::CurrencyEntry entry(static_cast<int>(OPL::CurrencyName::LineCheck), row_data);
    if (!DB->commit(entry))
        WARN(tr("Unable to update currency. The following error has ocurred:<br>%1").arg(DB->lastError.text()));

    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currMedDateEdit_userDateChanged(const QDate &date)
{
    const OPL::RowData_T row_data = {{OPL::Db::CURRENCIES_EXPIRYDATE, date.toString(Qt::ISODate)}};
    const OPL::CurrencyEntry entry(static_cast<int>(OPL::CurrencyName::Medical), row_data);
    if (!DB->commit(entry))
        WARN(tr("Unable to update currency. The following error has ocurred:<br>%1").arg(DB->lastError.text()));

    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currCustom1DateEdit_userDateChanged(const QDate &date)
{
    const OPL::RowData_T row_data = {{OPL::Db::CURRENCIES_EXPIRYDATE, date.toString(Qt::ISODate)},
                                {OPL::Db::CURRENCIES_CURRENCYNAME, ui->currCustom1LineEdit->text()}};
    const OPL::CurrencyEntry entry(static_cast<int>(OPL::CurrencyName::Custom1), row_data);
    DEB << entry;
    if (!DB->commit(entry))
        WARN(tr("Unable to update currency. The following error has ocurred:<br><br>%1").arg(DB->lastError.text()));

    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currCustom2DateEdit_userDateChanged(const QDate &date)
{
    const OPL::RowData_T row_data = {{OPL::Db::CURRENCIES_EXPIRYDATE, date.toString(Qt::ISODate)},
                                {OPL::Db::CURRENCIES_CURRENCYNAME, ui->currCustom2LineEdit->text()}};
    const OPL::CurrencyEntry entry(static_cast<int>(OPL::CurrencyName::Custom2), row_data);
    if (!DB->commit(entry))
        WARN(tr("Unable to update currency. The following error has ocurred:<br><br>%1").arg(DB->lastError.text()));

    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currToLdgCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        Settings::write(Settings::UserData::ShowToLgdCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        Settings::write(Settings::UserData::ShowToLgdCurrency, false);
        break;
    default:
        break;
    }
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currLicCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        Settings::write(Settings::UserData::ShowLicCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        Settings::write(Settings::UserData::ShowLicCurrency, false);
        break;
    default:
        break;
    }
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currTrCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        Settings::write(Settings::UserData::ShowTrCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        Settings::write(Settings::UserData::ShowTrCurrency, false);
        break;
    default:
        break;
    }
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currLckCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        Settings::write(Settings::UserData::ShowLckCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        Settings::write(Settings::UserData::ShowLckCurrency, false);
        break;
    default:
        break;
    }
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currMedCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        Settings::write(Settings::UserData::ShowMedCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        Settings::write(Settings::UserData::ShowMedCurrency, false);
        break;
    default:
        break;
    }
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currCustom1CheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        Settings::write(Settings::UserData::ShowCustom1Currency, true);
        break;
    case Qt::CheckState::Unchecked:
        Settings::write(Settings::UserData::ShowCustom1Currency, false);
        break;
    default:
        break;
    }
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currCustom2CheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        Settings::write(Settings::UserData::ShowCustom2Currency, true);
        break;
    case Qt::CheckState::Unchecked:
        Settings::write(Settings::UserData::ShowCustom2Currency, false);
        break;
    default:
        break;
    }
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currCustom1LineEdit_editingFinished()
{
    Settings::write(Settings::UserData::Custom1CurrencyName, ui->currCustom1LineEdit->text());
}

void SettingsWidget::on_currCustom2LineEdit_editingFinished()
{
    Settings::write(Settings::UserData::Custom2CurrencyName, ui->currCustom2LineEdit->text());
}

void SettingsWidget::on_languageComboBox_activated(int arg1)
{
    if (arg1 != 0) {
        INFO(tr("Translations are not yet available. If you are interested in making openPilotLog available in your native "
             "language, visit us <a href=\"https://%1/\">here</a> for more information."
             ).arg(QStringLiteral("github.com/fiffty-50/openpilotlog/wiki/Translations")));
        ui->languageComboBox->setCurrentIndex(0);
    }
}


void SettingsWidget::on_exportPushButton_clicked()
{
    auto exp = new ExportToCsvDialog(this);
    exp->exec();
}

