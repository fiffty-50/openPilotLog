/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include "src/database/adatabase.h"
#include "src/classes/apilotentry.h"
#include "src/opl.h"
#include "src/functions/adate.h"

static const auto FIRSTNAME_VALID = QPair<QString, QRegularExpression> {
    QStringLiteral("firstnameLineEdit"), QRegularExpression("[a-zA-Z]+")};
static const auto LASTNAME_VALID = QPair<QString, QRegularExpression> {
    QStringLiteral("lastnameLineEdit"), QRegularExpression("\\w+")};
static const auto PHONE_VALID = QPair<QString, QRegularExpression> {
    QStringLiteral("phoneLineEdit"), QRegularExpression("^[+]{0,1}[0-9\\-\\s]+")};
static const auto EMAIL_VALID = QPair<QString, QRegularExpression> {
    QStringLiteral("emailLineEdit"), QRegularExpression("\\A[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"
                                        "(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z")};
static const auto COMPANY_VALID = QPair<QString, QRegularExpression> {
    QStringLiteral("companyLineEdit"), QRegularExpression("\\w+")};
static const auto EMPLOYEENR_VALID = QPair<QString, QRegularExpression> {
    QStringLiteral("employeeidLineEdit"), QRegularExpression("\\w+")};
static const auto PREFIX_VALID = QPair<QString, QRegularExpression> {
    QStringLiteral("prefixLineEdit"), QRegularExpression("[a-zA-Z0-9]?[a-zA-Z0-9]?[a-zA-Z0-9]")};

static const auto LINE_EDIT_VALIDATORS = QVector<QPair<QString, QRegularExpression>>{
    FIRSTNAME_VALID, LASTNAME_VALID, PHONE_VALID, EMAIL_VALID,
    COMPANY_VALID, EMPLOYEENR_VALID, PREFIX_VALID};

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    ui->acAllowIncompleteComboBox->hide(); // [F]: Hidden for now, thinking of removing that option
    ui->acAllowIncompleteLabel->hide();

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
        // Style combo box
        const QSignalBlocker blocker_style(ui->styleComboBox);
        ui->styleComboBox->addItems(AStyle::styles);
        for (const auto &style_sheet : AStyle::styleSheets) {
            ui->styleComboBox->addItem(style_sheet.styleSheetName);
        }
        ui->styleComboBox->addItem(QStringLiteral("Dark-Palette"));
        ui->styleComboBox->model()->sort(0);

        // Approach Combo Box
        const QSignalBlocker blocker_approach(ui->approachComboBox);
        for (const auto &approach : Opl::ApproachTypes)
            ui->approachComboBox->addItem(approach);
        // Language Combo Box
        const QSignalBlocker blocker_language(ui->languageComboBox);
        for (const auto &lang : Opl::L10N_NAMES)
            ui->languageComboBox->addItem(lang);
    }
}


void SettingsWidget::setupDateEdits()
{
    // Read Display Format Setting
    int date_format_index = ASettings::read(ASettings::Main::DateFormat).toInt();
    const QString date_format_string = ADate::getFormatString(
                static_cast<Opl::Date::ADateFormat>(date_format_index));
    // Set Up Date Format Combo Box
    const QSignalBlocker blocker_date(ui->dateFormatComboBox);
    for (const auto &date_format : ADate::getDisplayNames())
        ui->dateFormatComboBox->addItem(date_format);
    ui->dateFormatComboBox->setCurrentIndex(date_format_index);
    const auto date_edits = this->findChildren<QDateEdit*>();
    for (const auto &date_edit : date_edits) {
        date_edit->setDisplayFormat(date_format_string);
    }
    // De-activate non-default date settings for now, implement in future release
    ui->dateFormatComboBox->setVisible(false);
    ui->dateFormatLabel->setVisible(false);
    // Fill currencies
    const QList<QPair<ACurrencyEntry::CurrencyName, QDateEdit* >> currencies = {
        {ACurrencyEntry::CurrencyName::Licence,     ui->currLicDateEdit},
        {ACurrencyEntry::CurrencyName::TypeRating,  ui->currTrDateEdit},
        {ACurrencyEntry::CurrencyName::LineCheck,   ui->currLckDateEdit},
        {ACurrencyEntry::CurrencyName::Medical,     ui->currMedDateEdit},
        {ACurrencyEntry::CurrencyName::Custom1,     ui->currCustom1DateEdit},
        {ACurrencyEntry::CurrencyName::Custom2,     ui->currCustom2DateEdit}
    };
    for (const auto &pair : currencies) {
        const QSignalBlocker signal_blocker(pair.second);
        const auto entry = aDB->getCurrencyEntry(pair.first);
        if (entry.isValid()) { // set date
            const auto date = QDate::fromString(
                        entry.tableData.value(Opl::Db::CURRENCIES_EXPIRYDATE).toString(),
                        Qt::ISODate);
            pair.second->setDate(date);
        } else { // set current date
            pair.second->setDate(QDate::currentDate());
        }
    }
}

/*!
 * \brief SettingsWidget::readSettings Reads settings from ASettings and sets up the UI accordingly
 */
void SettingsWidget::readSettings()
{
    /*
     * Personal Tab
     */
    {
        const QSignalBlocker blocker(this); // don't emit editing finished for setting these values
        auto user_data = aDB->getPilotEntry(1).getData();
        ui->lastnameLineEdit->setText(user_data.value(Opl::Db::PILOTS_LASTNAME).toString());
        ui->firstnameLineEdit->setText(user_data.value(Opl::Db::PILOTS_FIRSTNAME).toString());
        ui->companyLineEdit->setText(user_data.value(Opl::Db::PILOTS_COMPANY).toString());
        ui->employeeidLineEdit->setText(user_data.value(Opl::Db::PILOTS_EMPLOYEEID).toString());
        ui->phoneLineEdit->setText(user_data.value(Opl::Db::PILOTS_PHONE).toString());
        ui->emailLineEdit->setText(user_data.value(Opl::Db::PILOTS_EMAIL).toString());
    }

    /*
     * Flight Logging Tab
     */
    ui->aliasComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::DisplaySelfAs).toInt());
    ui->functionComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Function).toString());
    ui->rulesComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Rules).toString());
    ui->approachComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Approach).toString());
    ui->nightComboBox->setCurrentIndex(ASettings::read(ASettings::FlightLogging::NightLoggingEnabled).toInt());
    ui->prefixLineEdit->setText(ASettings::read(ASettings::FlightLogging::FlightNumberPrefix).toString());
    ui->logbookViewComboBox->setCurrentIndex(ASettings::read(ASettings::Main::LogbookView).toInt());

    /*
     * Currencies Tab
     */
    ui->currToLdgCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowToLgdCurrency).toBool());
    ui->currLicCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowLicCurrency).toBool());
    ui->currTrCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowTrCurrency).toBool());
    ui->currLckCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowLckCurrency).toBool());
    ui->currMedCheckBox->setChecked(ASettings::read(ASettings::UserData::ShowMedCurrency).toBool());
    ui->currCustom1CheckBox->setChecked(ASettings::read(ASettings::UserData::ShowCustom1Currency).toBool());
    ui->currCustom2CheckBox->setChecked(ASettings::read(ASettings::UserData::ShowCustom2Currency).toBool());
    ui->currCustom1LineEdit->setText(ASettings::read(ASettings::UserData::Custom1CurrencyName).toString());
    ui->currCustom2LineEdit->setText(ASettings::read(ASettings::UserData::Custom2CurrencyName).toString());

    /*
     * Misc Tab
     */
    ui->acftSortComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::TailSortColumn).toInt());
    ui->pilotSortComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::PilotSortColumn).toInt());
    //ui->acAllowIncompleteComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::AcftAllowIncomplete).toInt());
    {
        // Block style widgets signals to not trigger style changes during UI setup
        const QSignalBlocker style_blocker(ui->styleComboBox);
        const QSignalBlocker font_blocker1(ui->fontSpinBox);
        const QSignalBlocker font_blocker2(ui->fontComboBox);
        const QSignalBlocker font_blocker3(ui->fontCheckBox);
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
}

void SettingsWidget::setupValidators()
{
    // DEB << "Setting up Validators...";
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

/*!
 * \brief SettingsWidget::updatePersonalDetails Updates the database with the users personal details.
 */
void SettingsWidget::updatePersonalDetails()
{
    RowData_T user_data;
    switch (ui->aliasComboBox->currentIndex()) {
    case 0:
        user_data.insert(Opl::Db::PILOTS_ALIAS, QStringLiteral("self"));
        break;
    case 1:
        user_data.insert(Opl::Db::PILOTS_ALIAS, QStringLiteral("SELF"));
        break;
    case 2:{
        QString name;
        name.append(ui->lastnameLineEdit->text());
        name.append(QLatin1String(", "));
        name.append(ui->firstnameLineEdit->text().left(1));
        name.append(QLatin1Char('.'));
        user_data.insert(Opl::Db::PILOTS_ALIAS, name);
    }
        break;
    default:
        break;
    }
    user_data.insert(Opl::Db::PILOTS_LASTNAME, ui->lastnameLineEdit->text());
    user_data.insert(Opl::Db::PILOTS_FIRSTNAME, ui->firstnameLineEdit->text());
    user_data.insert(Opl::Db::PILOTS_COMPANY, ui->companyLineEdit->text());
    user_data.insert(Opl::Db::PILOTS_EMPLOYEEID, ui->employeeidLineEdit->text());
    user_data.insert(Opl::Db::PILOTS_PHONE, ui->phoneLineEdit->text());
    user_data.insert(Opl::Db::PILOTS_EMAIL, ui->emailLineEdit->text());

    auto user = APilotEntry(1);
    user.setData(user_data);

    aDB->commit(user);
}

/*
 * Slots
 */


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
    ASettings::write(ASettings::FlightLogging::Rules, arg1);
    ASettings::write(ASettings::FlightLogging::LogIFR, ui->rulesComboBox->currentIndex());
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

QT_DEPRECATED
void SettingsWidget::on_acAllowIncompleteComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::AcftAllowIncomplete, index);
    if (index) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, tr("Warning"),
                                      tr("Enabling incomplete logging will enable you to add aircraft with incomplete data.<br><br>"
                                      "If you do not fill out the aircraft details, "
                                      "it will be impossible to automatically determine Single/Multi Pilot Times or Single/Multi Engine Time. "
                                      "This will also impact statistics and auto-logging capabilites as well as jeopardise database integrity.<br><br>"
                                      "It is highly recommended to keep this option off unless you have a specific reason not to.<br><br>"
                                      "Are you sure you want to proceed?"),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            ASettings::write(ASettings::UserData::AcftAllowIncomplete, index);
        } else {
            ui->acAllowIncompleteComboBox->setCurrentIndex(0);
        }
    }
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
    QPixmap icon = QPixmap(Opl::Assets::ICON_MAIN);
    message_box.setIconPixmap(icon.scaledToWidth(64, Qt::TransformationMode::SmoothTransformation));
    QString SQLITE_VERSION = aDB->sqliteVersion();
    QString text = QMessageBox::tr(

                       "<h3><center>About openPilotLog</center></h3>"
                       "<br>"
                       "&#169; 2020-2021 Felix Turowsky"
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

                       "<p>This program uses <a href=\"http://%3/\">Qt</a> version %4 and "
                       "<a href=\"https://%5/\">SQLite</a> version %6</p>"
                   ).arg(QStringLiteral("github.com/fiffty-50/openpilotlog"),
                         QStringLiteral("gnu.org/licenses/"),
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
        return;
    }
    for (const auto &style_name : AStyle::styles) {
        if (new_style_setting == style_name) {
            AStyle::setStyle(style_name);
            ASettings::write(ASettings::Main::Style, new_style_setting);
            return;
        }
    }

    for (const auto &style_sheet : AStyle::styleSheets) {
        if (new_style_setting == style_sheet.styleSheetName) {
            AStyle::setStyle(style_sheet);
            ASettings::write(ASettings::Main::Style, new_style_setting);
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
    ACurrencyEntry entry(ACurrencyEntry::CurrencyName::Licence, date);
    aDB->commit(entry);
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currTrDateEdit_userDateChanged(const QDate &date)
{
    ACurrencyEntry entry(ACurrencyEntry::CurrencyName::TypeRating, date);
    aDB->commit(entry);
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currLckDateEdit_userDateChanged(const QDate &date)
{
    ACurrencyEntry entry(ACurrencyEntry::CurrencyName::LineCheck, date);
    aDB->commit(entry);
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currMedDateEdit_userDateChanged(const QDate &date)
{
    ACurrencyEntry entry(ACurrencyEntry::CurrencyName::Medical, date);
    aDB->commit(entry);
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currCustom1DateEdit_userDateChanged(const QDate &date)
{
    ACurrencyEntry entry(ACurrencyEntry::CurrencyName::Custom1, date);
    aDB->commit(entry);
    emit settingChanged(HomeWidget);
}

void SettingsWidget::on_currCustom2DateEdit_userDateChanged(const QDate &date)
{
    ACurrencyEntry entry(ACurrencyEntry::CurrencyName::Custom2, date);
    aDB->commit(entry);
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

void SettingsWidget::on_dateFormatComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::Main::DateFormat, index);
    const auto date_edits = this->findChildren<QDateEdit*>();
    for (const auto & date_edit : date_edits) {
        date_edit->setDisplayFormat(
                    ADate::getFormatString(
                        static_cast<Opl::Date::ADateFormat>(ASettings::read(ASettings::Main::DateFormat).toInt())));
    }
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

