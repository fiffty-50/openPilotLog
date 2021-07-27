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
#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QMessageBox>
#include <QStringBuilder>
#include <QDateEdit>

namespace Ui {
class FirstRunDialog;
}

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

    void on_currWarningCheckBox_stateChanged(int arg1);

    void on_currWarningThresholdSpinBox_valueChanged(int arg1);

    void on_currCustom1LineEdit_editingFinished();

    void on_currCustom2LineEdit_editingFinished();

    void on_dateFormatComboBox_currentIndexChanged(int index);

    /*!
     * \brief Import an existing database instead of creating a new one
     */
    void on_importPushButton_clicked();

private:
    Ui::FirstRunDialog *ui;
    bool useRessourceData;

    void writeSettings();
    bool setupDatabase();
    bool createUserEntry();
    bool writeCurrencies();
    bool finishSetup();
    bool downloadTemplates(QString branch_name);
    bool verifyTemplates();

    QList<QDateEdit*> dateEdits;

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
