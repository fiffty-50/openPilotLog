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
#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include "src/database/currencyentry.h"
#include <QDialog>
#include <QButtonGroup>
#include <QMessageBox>
#include <QStringBuilder>
#include <QDateEdit>

namespace Ui {
class FirstRunDialog;
}
/*!
 * \brief The FirstRunDialog is used as a set-up wizard for the application.
 * \details The Application does not ship with a database, it is created on the fly. This is the main purpose of the
 * FirstRunDialog. Apart from this main task, user details and preferences are gathered and saved using Settings.
 *
 * The Dialog contains a tabbed widget which guides the user through the set up process.
 *
 * ### 1 - Welcome
 *
 * In this tab, the user is greeted and has the option to import an existing database instead of creating a new one.
 *
 * ### 2 - Personal Data
 *
 * In this tab, the users personal data is collected, which is used to create the first Pilot Entry in the database.
 * ROW ID (1) identifies the logbook owner, which is also referred to as "self" in the application.
 *
 * ### 3 - Currencies
 *
 * In this tab, the user can enter the expiration dates of his licenses, medical or other currencies that he wishes to keep
 * track of. These are stored in the database within the currencies table.
 *
 * ### 4 - Flight Logging
 *
 * In this tab, the user can determine what the defualt selections for logging a new flight will be. These selections (Pilot Function,
 * Flight Rules,...) are stored in an INI file using the Settings class.
 *
 * ### 5 - Customization
 *
 * In this tab, the user can make selections regarding the application layout and theming, as well as with regards to how data is being
 * displayed. These settings are stored in an INI file using the Settings class.
 *
 * ### 6 - Finish / Database creation
 *
 * In this tab the user can select how the database is created. There are two options, either downloading database templates from the
 * github repository, or to fall back to a set of JSON files inclnuded in the application ressources (see templates.qrc). These built
 * in files are probably not as up-to-date, but enable the application to function if no internet connection is available or the links
 * to the online ressources are broken.
 *
 * For Debug purcposes, pressing <ctrl+t> will enable a line edit in which a different branch than main can be selected from which the
 * database templates are to be pulled.
 *
 *
 *
 * Upon successful completion of the FirstRunDialog, a Setting is written so that the dialog is not shown on subsequent runs.
 */
class FirstRunDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FirstRunDialog(QWidget *parent = nullptr);
    ~FirstRunDialog();

private slots:

    void on_previousPushButton_clicked();
    void on_nextPushButton_clicked();
    void on_styleComboBox_currentTextChanged(const QString &new_style_setting);

    /*!
     * \brief Import an existing database instead of creating a new one
     */
    void on_importPushButton_clicked();

private:
    Ui::FirstRunDialog *ui;
    bool useRessourceData;

    //TODO load from settings
    OPL::DateTimeFormat m_format = OPL::DateTimeFormat(
        OPL::DateTimeFormat::DateFormat::Default,
        QStringLiteral("yyyy-MM-dd"),
        OPL::DateTimeFormat::TimeFormat::Default,
        QStringLiteral("hh:mm")
        );

    /*!
     * \brief finishSetup - once all the necessary data is entered by the user, this functions executes the steps necessary
     * to collect the data, process it and create the database
     */
    bool finishSetup();

    /*!
     * \brief writeSettings - collects input from the UI and writes the user selections to an INI file
     */
    void writeSettings();

    /*!
     * \brief setupDatabase - creates the sqlite database
     */
    bool setupDatabase();
    /*!
     * \brief createUserEntry - create the database entry for the logbook owner (identified by ROW_ID = 1)
     */
    bool createUserEntry();

    /*!
     * \brief setupPreviousExperienceEntry - set up a stub database entry for previous experience
     */
    bool setupPreviousExperienceEntry();

    /*!
     * \brief writeCurrencies - submits the user input to the currencies table in the database
     */
    bool writeCurrencies();

    /*!
     * \brief downloadTemplates - Downloads the airports and aircraft database as JSON files from github
     */
    bool downloadTemplates(QString branch_name);
    /*!
     * \brief verifyTemplates - run a MD5 checksum hash
     */
    bool verifyTemplates();

protected:
    void reject() override;
    /*!
     * \brief Shows the debug widget by pressing <ctrl + t>
     */

    /*!
     * \brief keyPressEvent ctrl + t enables debug mode, showing the branch selector
     * which is used to select a git branch other than main for downloading the templates
     * \param keyEvent
     */
    void keyPressEvent(QKeyEvent* keyEvent) override;
};

#endif // FIRSTRUNDIALOG_H
