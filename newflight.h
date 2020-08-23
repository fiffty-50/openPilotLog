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
#ifndef NEWFLIGHT_H
#define NEWFLIGHT_H

#include <QDialog>

namespace Ui {
class NewFlight;
}

class NewFlight : public QDialog
{
    Q_OBJECT

public:
    explicit NewFlight(QWidget *parent = nullptr);
    ~NewFlight();

private slots:
    void nope();//error box

    QString validateTimeInput(QString userinput);

    QVector<QString> collectInput();

    void fillExtrasLineEdits();

    void storeSettings();

    void restoreSettings();

    bool verifyInput();

    void returnInput(QVector<QString> flight);

    void on_deptTZ_currentTextChanged(const QString &arg1);

    void on_destTZ_currentIndexChanged(const QString &arg1);

    void on_newDept_textEdited(const QString &arg1);

    void on_newDept_editingFinished();

    void on_newDest_textEdited(const QString &arg1);

    void on_newDest_editingFinished();

    void on_newDoft_editingFinished();

    void on_newTofb_editingFinished();

    void on_newTonb_editingFinished();

    void on_newAcft_textEdited(const QString &arg1);

    void on_newAcft_editingFinished();

    void on_newPic_textEdited(const QString &arg1);

    void on_newPic_editingFinished();

    void on_verifyButton_clicked();






    void on_setAsDefaultButton_clicked();

    void on_restoreDefaultButton_clicked();


    void on_buttonBox_accepted();

    void on_buttonBox_rejected();



    void on_PilotFlyingCheckBox_stateChanged(int);

    void on_ApproachComboBox_currentTextChanged(const QString &arg1);





    void on_tabWidget_tabBarClicked(int index);

    void on_spseTimeLineEdit_editingFinished();

    void on_spmeTimeLineEdit_editingFinished();

    void on_mpTimeLineEdit_editingFinished();

    void on_totalTimeLineEdit_editingFinished();

    void on_ifrTimeLineEdit_editingFinished();

    void on_vfrTimeLineEdit_editingFinished();

    void on_nightTimeLineEdit_editingFinished();

    void on_xcTimeLineEdit_editingFinished();

    void on_picTimeLineEdit_editingFinished();

    void on_copTimeLineEdit_editingFinished();

    void on_dualTimeLineEdit_editingFinished();

    void on_fiTimeLineEdit_editingFinished();

    void on_simTimeLineEdit_editingFinished();

private:
    Ui::NewFlight *ui;
};

#endif // NEWFLIGHT_H
