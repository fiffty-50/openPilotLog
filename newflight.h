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
#include <QRegularExpression>
#include <QMessageBox>
#include <QDebug>
#include <QCompleter>
#include <QLatin1Char>
#include <QStringList>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QButtonGroup>
#include "newacft.h"
#include "calc.h"
#include "dbsettings.h"
#include "dbflight.h"
#include "dbpilots.h"
#include "dbairport.h"
#include "dbaircraft.h"
#include "strictregularexpressionvalidator.h"

namespace Ui {
class NewFlight;
}

class NewFlight : public QDialog
{
    Q_OBJECT

public:
    explicit NewFlight(QWidget *parent, QStringList locationList,
                                        QStringList registrationList,
                                        QStringList pilotNameList);
    ~NewFlight();

private slots:
    void nope();//error box

    void on_verifyButton_clicked(); //debug button

    QVector<QString> collectInput();

    void fillExtrasLineEdits();

    void storeSettings();

    void restoreSettings();

    bool verifyInput();

    void returnInput(QVector<QString> flight);

    void on_deptTZ_currentTextChanged(const QString &arg1);

    void on_destTZ_currentIndexChanged(const QString &arg1);

    void on_newDeptTimeLineEdit_editingFinished();

    void on_newDestTimeLineEdit_editingFinished();

    void on_newDeptLocLineEdit_editingFinished();

    void on_newDestLocLineEdit_editingFinished();

    void on_newDeptLocLineEdit_textEdited(const QString &arg1);

    void on_newDestLocLineEdit_textEdited(const QString &arg1);

    void on_newDoft_editingFinished();

    void on_newAcft_inputRejected();

    void on_newAcft_editingFinished();

    void on_newPicNameLineEdit_inputRejected();

    void on_newPicNameLineEdit_textEdited(const QString &arg1);

    void on_newPicNameLineEdit_editingFinished();

    void on_setAsDefaultButton_clicked();

    void on_restoreDefaultButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_PilotFlyingCheckBox_stateChanged(int);

    void on_ApproachComboBox_currentTextChanged(const QString &arg1);

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

    void on_secondPilotNameLineEdit_textEdited(const QString &arg1);

    void on_secondPilotNameLineEdit_editingFinished();

    void on_thirdPilotNameLineEdit_textEdited(const QString &arg1);

    void on_thirdPilotNameLineEdit_editingFinished();

    void on_FlightNumberLineEdit_editingFinished();



    void on_newDeptLocLineEdit_inputRejected();

    void on_newDestLocLineEdit_inputRejected();

    void on_newDeptTimeLineEdit_inputRejected();

    void on_newDestTimeLineEdit_inputRejected();



    void on_tabWidget_currentChanged(int index);



private:
    Ui::NewFlight *ui;
};

#endif // NEWFLIGHT_H
