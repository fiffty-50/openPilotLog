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
#include "src/testing/adebug.h"
#include "src/classes/astyle.h"
#include "src/classes/asettings.h"
#include "src/database/adatabase.h"
#include "src/classes/apilotentry.h"
#include "src/oplconstants.h"

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

    setupComboBoxes();
    setupValidators();
    readSettings();
}


SettingsWidget::~SettingsWidget()
{
    delete ui;
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
        for (const auto &approach : Opl::ApproachTypes) {
            ui->approachComboBox->addItem(approach);
        }
    }
}

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
    ui->nightComboBox->setCurrentIndex(ASettings::read(ASettings::FlightLogging::NightLogging).toInt());
    ui->prefixLineEdit->setText(ASettings::read(ASettings::FlightLogging::FlightNumberPrefix).toString());

    ui->logbookViewComboBox->setCurrentIndex(ASettings::read(ASettings::Main::LogbookView).toInt());
    /*
     * Misc Tab
     */
    ui->acSortComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::AcftSortColumn).toInt());
    ui->pilotSortComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::PilSortColumn).toInt());
    ui->acAllowIncompleteComboBox->setCurrentIndex(ASettings::read(ASettings::UserData::AcAllowIncomplete).toInt());
    ui->styleComboBox->setCurrentText(ASettings::read(ASettings::Main::Style).toString());
    {
        const QSignalBlocker font_blocker1(ui->fontSpinBox);
        const QSignalBlocker font_blocker2(ui->fontComboBox);
        const QSignalBlocker font_blocker3(ui->fontCheckBox);
        ui->fontSpinBox->setValue(ASettings::read(ASettings::Main::FontSize).toUInt());
        ui->fontComboBox->setCurrentFont(QFont(ASettings::read(ASettings::Main::Font).toString()));
        ui->fontCheckBox->setChecked(ASettings::read(ASettings::Main::UseSystemFont).toBool());
    }
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
    ASettings::write(ASettings::Main::LogbookView, index);
    emit viewSelectionChanged(index);
}
void SettingsWidget::on_pilotSortComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::PilSortColumn, index);
}

void SettingsWidget::on_acSortComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::AcftSortColumn, index);
}

void SettingsWidget::on_acAllowIncompleteComboBox_currentIndexChanged(int index)
{
    ASettings::write(ASettings::UserData::AcAllowIncomplete, index);
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
    QMessageBox message_box(this);
    QString SQLITE_VERSION = aDB->sqliteVersion();
    QString text = QMessageBox::tr(

                       "<h3><center>About openPilotLog</center></h3>"
                       "<br>"
                       "(c) 2020-2021 Felix Turowsky"
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
        QMessageBox message_box(this);
        message_box.setText(tr("The style you have currently selected may not be fully compatible "
                               "with changing to a custom font while the application is running.<br><br>"
                               "Applying your changes may require restarting the application.<br>"));
        message_box.exec();
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
        DEB << "Setting Font:" << font.toString();
        break;
    }
    case Qt::Checked:
    {
        ui->fontComboBox->setEnabled(false);
        ui->fontSpinBox->setEnabled(false);
        ASettings::write(ASettings::Main::UseSystemFont, true);
        QMessageBox message_box(this);
        message_box.setText(tr("The application will be restarted for this change to take effect."));
        message_box.exec();
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
    DEB << "Resetting style to default...";
    ui->styleComboBox->setCurrentText(AStyle::defaultStyle);
    ui->fontCheckBox->setChecked(true);
}
