/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
#include "src/functions/alog.h"
#include "src/classes/astyle.h"
#include "src/classes/asettings.h"
#include "src/database/database.h"
#include "src/database/row.h"
#include "src/opl.h"
#include "src/functions/adate.h"
#include "src/gui/widgets/backupwidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    loadBackupWidget();
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
        AStyle::loadStylesComboBox(ui->styleComboBox);
        OPL::GLOBALS->loadApproachTypes(ui->approachComboBox);
        OPL::GLOBALS->loadPilotFunctios(ui->functionComboBox);
        OPL::GLOBALS->fillViewNamesComboBox(ui->logbookViewComboBox);
        OPL::GLOBALS->fillLanguageComboBox(ui->languageComboBox);
    }
}


void SettingsWidget::setupDateEdits()
{
    // Read Display Format Setting
    int date_format_index = ASettings::read(ASettings::Main::DateFormat).toInt();
    const QString date_format_string = ADate::getFormatString(
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

/*!
 * \brief SettingsWidget::readSettings Reads settings from ASettings and sets up the UI accordingly
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
    ui->functionComboBox->setCurrentIndex(ASettings::read(ASettings::FlightLogging::Function).toInt());
    ui->rulesComboBox->setCurrentIndex(ASettings::read(ASettings::FlightLogging::LogIFR).toInt());
    ui->approachComboBox->setCurrentIndex(ASettings::read(ASettings::FlightLogging::Approach).toInt());
    ui->nightComboBox->setCurrentIndex(ASettings::read(ASettings::FlightLogging::NightLoggingEnabled).toInt());
    ui->prefixLineEdit->setText(ASettings::read(ASettings::FlightLogging::FlightNumberPrefix).toString());

    ui->logbookViewComboBox->setCurrentIndex(ASettings::read(ASettings::Main::LogbookView).toInt());
    ui->aliasComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::DisplaySelfAs).toInt());

    // Currencies Tab
    ui->currToLdgCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowToLgdCurrency).toBool());
    ui->currLicCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowLicCurrency).toBool());
    ui->currTrCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowTrCurrency).toBool());
    ui->currLckCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowLckCurrency).toBool());
    ui->currMedCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowMedCurrency).toBool());
    ui->currCustom1CheckBox->setChecked(ASettings::read(ASettings::UserData::ShowCustom1Currency).toBool());
    ui->currCustom2CheckBox->setChecked(ASettings::read(ASettings::UserData::ShowCustom2Currency).toBool());
    ui->currCustom1LineEdit->setText(ASettings::read(ASettings::UserData::Custom1CurrencyName).toString());
    ui->currCustom2LineEdit->setText(ASettings::read(ASettings::UserData::Custom2CurrencyName).toString());

    // Misc Tab
    ui->acftSortComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::TailSortColumn).toInt());
    ui->pilotSortComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::PilotSortColumn).toInt());

    // Don't emit signals for style changes during setup
    const QSignalBlocker style_blocker(ui->styleComboBox);
    const QSignalBlocker font_blocker_1(ui->fontSpinBox);
    const QSignalBlocker font_blocker_2(ui->fontComboBox);
    const QSignalBlocker font_blocker_3(ui->fontCheckBox);

    ui->styleComboBox->setCurrentText(ASettings::read(ASettings::Main::Style).toString());
    ui->fontSpinBox->setValue(ASettings::read(ASettings::Main::FontSize).toUInt());
    ui->fontComboBox->setCurrentFont(QFont(ASettings::read(ASettings::Main::Font).toString()));
    bool use_system_font = ASettings::read(ASettings::Main::UseSystemFont).toBool();
    ui->fontCheckBox->setChecked(use_system_font);
    if (!use_system_font) {
        ui->fontComboBox->setEnabled(true);
        ui->fontSpinBox->setEnabled(true);
    }
}

void SettingsWidget::setupValidators()
{
    const QHash<QLineEdit*, QRegularExpression> validator_map = {
        {ui->firstnameLineEdit, QRegularExpression(QLatin1String("\\w+"))},
        {ui->lastnameLineEdit, QRegularExpression(QLatin1String("\\w+"))},
        {ui->phoneLineEdit, QRegularExpression(QLatin1String("^[+]{0,1}[0-9\\-\\s]+"))},
        {ui->emailLineEdit, QRegularExpression(QString("\\A[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"
         "(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z"))},
        {ui->companyLineEdit, QRegularExpression(QLatin1String("\\w+"))},
        {ui->employeeidLineEdit, QRegularExpression(QLatin1String("\\w+"))},
        {ui->prefixLineEdit, QRegularExpression(QLatin1String("\\w+"))},
    };

    QHash<QLineEdit*, QRegularExpression>::const_iterator i;
    for (i = validator_map.constBegin(); i != validator_map.constEnd(); ++i) {
        auto validator = new QRegularExpressionValidator(i.value(),i.key());
        i.key()->setValidator(validator);
    }
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
    ASettings::write(ASettings::UserData::DisplaySelfAs, index);
    updatePersonalDetails();
}

void SettingsWidget::on_functionComboBox_currentIndexChanged(int arg1)
{
    ASettings::write(ASettings::FlightLogging::Function, arg1);
}

void SettingsWidget::on_rulesComboBox_currentIndexChanged(int arg1)
{
    ASettings::write(ASettings::FlightLogging::LogIFR, arg1);
}

void SettingsWidget::on_approachComboBox_currentIndexChanged(int arg1)
{
    ASettings::write(ASettings::FlightLogging::Approach, arg1);
}

void SettingsWidget::on_nightComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::FlightLogging::NightLoggingEnabled, index);
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
    ASettings::write(ASettings::Main::LogbookView, index);
    emit settingChanged(SettingSignal::LogbookWidget);
}
void SettingsWidget::on_pilotSortComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::PilotSortColumn, index);
    emit settingChanged(PilotsWidget);
}

void SettingsWidget::on_acftSortComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::TailSortColumn, index);
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

void SettingsWidget::on_styleComboBox_currentTextChanged(const QString& new_style_setting)
{
    if (new_style_setting == QLatin1String("Dark-Palette")) {
        AStyle::setStyle(AStyle::darkPalette());
        ASettings::write(ASettings::Main::Style, new_style_setting);
        emit settingChanged(MainWindow);
        return;
    }
    for (const auto &style_name : AStyle::styles) {
        if (new_style_setting == style_name) {
            AStyle::setStyle(style_name);
            ASettings::write(ASettings::Main::Style, new_style_setting);
            emit settingChanged(MainWindow);
            return;
        }
    }

    for (const auto &style_sheet : AStyle::styleSheets) {
        if (new_style_setting == style_sheet.styleSheetName) {
            AStyle::setStyle(style_sheet);
            ASettings::write(ASettings::Main::Style, new_style_setting);
            emit settingChanged(MainWindow);
            return;
        }
    }
}

void SettingsWidget::on_fontComboBox_currentFontChanged(const QFont &f)
{
    qApp->setFont(f);
    ASettings::write(ASettings::Main::Font, f.toString());
    DEB << "Setting Font:" << f.toString();
}

void SettingsWidget::on_fontSpinBox_valueChanged(int arg1)
{
    QFont f = qApp->font();
    f.setPointSize(arg1);
    qApp->setFont(f);
    ASettings::write(ASettings::Main::FontSize, arg1);
    DEB << "Setting Font:" << f.toString();
}

void SettingsWidget::on_fontCheckBox_stateChanged(int arg1)
{
    if (usingStylesheet() && arg1 == Qt::Unchecked) {
        WARN(tr("The style you have currently selected may not be fully compatible "
                "with changing to a custom font while the application is running.<br><br>"
                "Applying your changes may require restarting the application.<br>"));
    }
    switch (arg1) {
    case Qt::Unchecked:
    {
        ui->fontComboBox->setEnabled(true);
        ui->fontSpinBox->setEnabled(true);
        ASettings::write(ASettings::Main::UseSystemFont, false);
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
        ASettings::write(ASettings::Main::UseSystemFont, true);
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
 * \brief Determines if the user has selected a stylesheet or is using a Qt Style Factory Style
 */
bool SettingsWidget::usingStylesheet()
{
    for (const auto &style_sheet : AStyle::styleSheets) {
        if (style_sheet.styleSheetName == ui->styleComboBox->currentText())
            return true;
    }
    return false;
}

void SettingsWidget::on_resetStylePushButton_clicked()
{
    LOG << "Resetting style to default...";
    ui->styleComboBox->setCurrentText(AStyle::defaultStyle);
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
        ASettings::write(ASettings::UserData::ShowToLgdCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::ShowToLgdCurrency, false);
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
        ASettings::write(ASettings::UserData::ShowLicCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::ShowLicCurrency, false);
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
        ASettings::write(ASettings::UserData::ShowTrCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::ShowTrCurrency, false);
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
        ASettings::write(ASettings::UserData::ShowLckCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::ShowLckCurrency, false);
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
        ASettings::write(ASettings::UserData::ShowMedCurrency, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::ShowMedCurrency, false);
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
        ASettings::write(ASettings::UserData::ShowCustom1Currency, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::ShowCustom1Currency, false);
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
        ASettings::write(ASettings::UserData::ShowCustom2Currency, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::ShowCustom2Currency, false);
        break;
    default:
        break;
    }
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currCustom1LineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Custom1CurrencyName, ui->currCustom1LineEdit->text());
}

void SettingsWidget::on_currCustom2LineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Custom2CurrencyName, ui->currCustom2LineEdit->text());
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

