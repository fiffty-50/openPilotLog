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
#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QValidator>
#include <QProcess>
#include <QDebug>
#include <QFontDialog>

namespace Ui {
class SettingsWidget;
}

/*!
 * \brief The SettingsWidget is used to to display and alter Settings.
 *
 * \details Most Inputs are collected and processed in various slots and
 * written to the settings file via the ASettings class. In the `Personal` Settings
 * tab, the user can edit his personal details, which are then written to the Database
 * (The Logbook owner is registered in the Pilots Database with `pilot_id = 1`).
 */
class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

    /*!
     * \brief enumerates Widgets that need to receive a signal when a setting is updated.
     */
    enum SettingSignal {LogbookWidget, HomeWidget, AircraftWidget, PilotsWidget};

private slots:

    void on_aboutPushButton_clicked();
    void on_acftSortComboBox_currentIndexChanged(int index);
    void on_acAllowIncompleteComboBox_currentIndexChanged(int index);
    void on_prefixLineEdit_textChanged(const QString &arg1);
    void on_lastnameLineEdit_editingFinished();
    void on_firstnameLineEdit_editingFinished();
    void on_employeeidLineEdit_editingFinished();
    void on_emailLineEdit_editingFinished();
    void on_phoneLineEdit_editingFinished();
    void on_aliasComboBox_currentIndexChanged(int index);
    void on_functionComboBox_currentIndexChanged(const QString &arg1);
    void on_rulesComboBox_currentIndexChanged(const QString &arg1);
    void on_approachComboBox_currentIndexChanged(const QString &arg1);
    void on_nightComboBox_currentIndexChanged(int index);
    void on_pilotSortComboBox_currentIndexChanged(int index);
    void on_logbookViewComboBox_currentIndexChanged(int index);
    void on_companyLineEdit_editingFinished();
    void on_styleComboBox_currentTextChanged(const QString& new_style_setting);

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_fontSpinBox_valueChanged(int arg1);

    void on_fontCheckBox_stateChanged(int arg1);

    void on_resetStylePushButton_clicked();

    void on_currLicDateEdit_userDateChanged(const QDate &date);

    void on_currTrDateEdit_userDateChanged(const QDate &date);

    void on_currLckDateEdit_userDateChanged(const QDate &date);

    void on_currMedDateEdit_userDateChanged(const QDate &date);

    void on_currCustom1DateEdit_userDateChanged(const QDate &date);

    void on_currCustom2DateEdit_userDateChanged(const QDate &date);

    void on_currToLdgCheckBox_stateChanged(int arg1);

    void on_currLicCheckBox_stateChanged(int arg1);

    void on_currTrCheckBox_stateChanged(int arg1);

    void on_currLckCheckBox_stateChanged(int arg1);

    void on_currMedCheckBox_stateChanged(int arg1);

    void on_currCustom1CheckBox_stateChanged(int arg1);

    void on_currCustom2CheckBox_stateChanged(int arg1);

    void on_currWarningCheckBox_stateChanged(int arg1);

    void on_currWarningThresholdSpinBox_valueChanged(int arg1);

    void on_currCustom1LineEdit_editingFinished();

    void on_currCustom2LineEdit_editingFinished();

    void on_dateFormatComboBox_currentIndexChanged(int index);

    void on_languageComboBox_activated(const QString &arg1);

private:
    Ui::SettingsWidget *ui;

    void readSettings();

    void setupValidators();

    void setupComboBoxes();

    void setupDateEdits();

    void updatePersonalDetails();

    bool usingStylesheet();

signals:

    /*!
     * \brief settingChanged is emitted when a setting change occurs that needs to trigger
     * an update (repaint) to another widget.
     */
    void settingChanged(SettingSignal widget);

protected:
    /*!
     * \brief Handles change events, like updating the UI to new localisation
     */
    void changeEvent(QEvent* event) override;
};

#endif // SETTINGSWIDGET_H
