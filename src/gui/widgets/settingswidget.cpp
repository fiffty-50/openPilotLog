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
#include "src/opl.h"
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

        // Set up the currency warning threshold spin box
        ui->currencyWarningDaysSpinBox->setValue(Settings::getCurrencyWarningThreshold());
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
    const auto user_data = DB->getLogbookOwner().getData();
    QString lastName = user_data.value(OPL::PilotEntry::LASTNAME).toString();
    if(lastName.isEmpty()) {
        lastName = "Please enter your last name.";
    }
    ui->lastnameLineEdit->setText(lastName);
    ui->firstnameLineEdit->setText(user_data.value(OPL::PilotEntry::FIRSTNAME).toString());
    ui->companyLineEdit->setText(user_data.value(OPL::PilotEntry::COMPANY).toString());
    ui->employeeidLineEdit->setText(user_data.value(OPL::PilotEntry::EMPLOYEEID).toString());
    ui->phoneLineEdit->setText(user_data.value(OPL::PilotEntry::PHONE).toString());
    ui->emailLineEdit->setText(user_data.value(OPL::PilotEntry::EMAIL).toString());

    // FLight Logging Tab
    ui->functionComboBox->setCurrentIndex(static_cast<int>(Settings::getPilotFunction()));
    ui->rulesComboBox->setCurrentIndex(Settings::getLogIfr());
    ui->approachComboBox->setCurrentText(Settings::getApproachType());
    ui->nightComboBox->setCurrentIndex(Settings::getNightLoggingEnabled());
    ui->prefixLineEdit->setText(Settings::getFlightNumberPrefix());

    ui->logbookViewComboBox->setCurrentIndex(static_cast<int>(Settings::getLogbookView()));
    ui->aliasComboBox->setCurrentIndex(Settings::getShowSelfAs());

    // Misc Tab
    ui->acftSortComboBox->setCurrentIndex(Settings::getTailSortColumn());
    ui->pilotSortComboBox->setCurrentIndex(Settings::getPilotSortColumn());

    // Don't emit signals for OPL::Style changes during setup
    const QSignalBlocker style_blocker(ui->styleComboBox);
    const QSignalBlocker font_blocker_1(ui->fontSpinBox);
    const QSignalBlocker font_blocker_2(ui->fontComboBox);
    const QSignalBlocker font_blocker_3(ui->fontCheckBox);

    ui->styleComboBox->setCurrentText(Settings::getApplicationStyle());
    ui->fontSpinBox->setValue(Settings::getApplicationFontSize());
    ui->fontComboBox->setCurrentFont(QFont(Settings::getApplicationFontName()));
    bool use_system_font = Settings::getUseSystemFont();
    ui->fontCheckBox->setChecked(use_system_font);
    if (!use_system_font) {
        ui->fontComboBox->setEnabled(true);
        ui->fontSpinBox->setEnabled(true);
    }
}

void SettingsWidget::setupValidators()
{
    ui->phoneLineEdit->setValidator(new QRegularExpressionValidator(OPL::RegEx::RX_PHONE_NUMBER, ui->phoneLineEdit));
}

/*!
 * \brief SettingsWidget::updatePersonalDetails Updates the database with the users personal details.
 */
void SettingsWidget::updatePersonalDetails()
{
    OPL::RowData_T user_data;
    switch (ui->aliasComboBox->currentIndex()) {
    case 0:
        user_data.insert(OPL::PilotEntry::ALIAS, QStringLiteral("self"));
        break;
    case 1:
        user_data.insert(OPL::PilotEntry::ALIAS, QStringLiteral("SELF"));
        break;
    case 2:{
        QString name;
        name.append(ui->lastnameLineEdit->text());
        if(ui->firstnameLineEdit->text().size() > 0) {
            name.append(QLatin1String(", "));
            name.append(ui->firstnameLineEdit->text().at(0));
            name.append(QLatin1Char('.'));
        }
        user_data.insert(OPL::PilotEntry::ALIAS, name);
    }
        break;
    default:
        break;
    }
    user_data.insert(OPL::PilotEntry::LASTNAME, ui->lastnameLineEdit->text());
    user_data.insert(OPL::PilotEntry::FIRSTNAME, ui->firstnameLineEdit->text());
    user_data.insert(OPL::PilotEntry::COMPANY, ui->companyLineEdit->text());
    user_data.insert(OPL::PilotEntry::EMPLOYEEID, ui->employeeidLineEdit->text());
    user_data.insert(OPL::PilotEntry::PHONE, ui->phoneLineEdit->text());
    user_data.insert(OPL::PilotEntry::EMAIL, ui->emailLineEdit->text());

    if(!DB->setLogbookOwner(user_data))
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
    Settings::setShowSelfAs(index);
    updatePersonalDetails();
}

void SettingsWidget::on_functionComboBox_currentIndexChanged(int arg1)
{
    Settings::setPilotFunction(OPL::PilotFunction(arg1));
}

void SettingsWidget::on_rulesComboBox_currentIndexChanged(int arg1)
{
    Settings::setLogIfr(arg1);
}

void SettingsWidget::on_approachComboBox_currentIndexChanged(int arg1)
{
    Settings::setApproachType(ui->approachComboBox->currentText());
}

void SettingsWidget::on_nightComboBox_currentIndexChanged(int index)
{
    Settings::setNightLoggingEnabled(index);
    switch (index) {
    case 1:
        Settings::setNightAngle(-6);
        break;
    case 2:
        Settings::setNightAngle(0);
        break;
    default:
        Settings::setNightAngle(-6);
    }
}

void SettingsWidget::on_prefixLineEdit_textChanged(const QString &arg1)
{
    Settings::setFlightNumberPrefix(arg1);
}

/*
 * Misc Tab
 */

void SettingsWidget::on_logbookViewComboBox_currentIndexChanged(int index)
{
//    Settings::write(Settings::Main::LogbookView, index);
    Settings::setLogbookView(OPL::LogbookView(index));
    emit settingChanged(SettingSignal::LogbookWidget);
}
void SettingsWidget::on_pilotSortComboBox_currentIndexChanged(int index)
{
    Settings::setPilotSortColumn(index);
    emit settingChanged(PilotsWidget);
}

void SettingsWidget::on_acftSortComboBox_currentIndexChanged(int index)
{
    Settings::setTailSortColumn(index);
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
//        Settings::write(Settings::Main::Style, new_style_setting);
        Settings::setApplicationStyle(new_style_setting);
        emit settingChanged(MainWindow);
        return;
    }
    for (const auto &style_name : OPL::Style::styles) {
        if (new_style_setting == style_name) {
            OPL::Style::setStyle(style_name);
//            Settings::write(Settings::Main::Style, new_style_setting);
            Settings::setApplicationStyle(style_name);
            emit settingChanged(MainWindow);
            return;
        }
    }

    for (const auto &style_sheet : OPL::Style::styleSheets) {
        if (new_style_setting == style_sheet.styleSheetName) {
            OPL::Style::setStyle(style_sheet);
//            Settings::write(Settings::Main::Style, new_style_setting);
            Settings::setApplicationStyle(new_style_setting);
            emit settingChanged(MainWindow);
            return;
        }
    }
}

void SettingsWidget::on_fontComboBox_currentFontChanged(const QFont &f)
{
    qApp->setFont(f);
//    Settings::write(Settings::Main::Font, f.toString());
    Settings::setApplicationFontName(f.toString());
    DEB << "Setting Font:" << f.toString();
}

void SettingsWidget::on_fontSpinBox_valueChanged(int arg1)
{
    QFont f = qApp->font();
    f.setPointSize(arg1);
    qApp->setFont(f);
//    Settings::write(Settings::Main::FontSize, arg1);
    Settings::setApplicationFontSize(arg1);
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
        Settings::setUseSystemFont(false);
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
        Settings::setUseSystemFont(true);
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


void SettingsWidget::on_currencyWarningDaysSpinBox_valueChanged(int arg1)
{
    Settings::setCurrencyWarningThreshold(arg1);
}

