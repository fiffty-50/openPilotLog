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
 * written to the settings file via the Settings class. In the `Personal` Settings
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
    enum SettingSignal {LogbookWidget, HomeWidget, AircraftWidget, PilotsWidget, MainWindow};

private slots:

    void on_aboutPushButton_clicked();
    void on_aboutBackupsPushButton_clicked();
    void on_acftSortComboBox_currentIndexChanged(int index);
    void on_prefixLineEdit_textChanged(const QString &arg1);
    void on_lastnameLineEdit_editingFinished();
    void on_firstnameLineEdit_editingFinished();
    void on_employeeidLineEdit_editingFinished();
    void on_emailLineEdit_editingFinished();
    void on_phoneLineEdit_editingFinished();
    void on_aliasComboBox_currentIndexChanged(int index);
    void on_functionComboBox_currentIndexChanged(int arg1);
    void on_rulesComboBox_currentIndexChanged(int arg1);
    void on_approachComboBox_currentIndexChanged(int arg1);
    void on_nightComboBox_currentIndexChanged(int index);
    void on_pilotSortComboBox_currentIndexChanged(int index);
    void on_logbookViewComboBox_currentIndexChanged(int index);
    void on_companyLineEdit_editingFinished();
    void on_styleComboBox_currentTextChanged(const QString& new_style_setting);
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_fontSpinBox_valueChanged(int arg1);
    void on_fontCheckBox_stateChanged(int arg1);
    void on_resetStylePushButton_clicked();
    void on_languageComboBox_activated(int arg1);
    void on_exportPushButton_clicked();

    void on_currencyWarningDaysSpinBox_valueChanged(int arg1);

private:
    Ui::SettingsWidget *ui;

    void readSettings();

    void setupValidators();

    void setupComboBoxes();

    void loadBackupWidget();

    void loadPreviousExperienceWidget();

    void updatePersonalDetails();

    bool usingStylesheet();

    const static int SELF_ROW_ID = 1;

signals:

    /*!
     * \brief settingChanged is emitted when a setting change occurs that needs to trigger
     * an update (repaint) to another widget.
     */
    void settingChanged(SettingsWidget::SettingSignal widget);

protected:
    /*!
     * \brief Handles change events, like updating the UI to new localisation
     */
    void changeEvent(QEvent* event) override;
};

#endif // SETTINGSWIDGET_H
