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
#include <QBitArray>
#include <QLineEdit>


#include "src/database/db.h"
#include "src/classes/flight.h"
#include "src/classes/aircraft.h"
#include "src/classes/strictrxvalidator.h"
#include "src/classes/settings.h"
#include "src/classes/completionlist.h"
#include "src/classes/calc.h"

#include "src/gui/dialogues/newpilot.h"

class SqlColumnNum{
public:
    SqlColumnNum() : _column(-1) {}
    explicit
    SqlColumnNum(int column) : _column(column) {}
    int column() const { return _column; }
private:
    int _column;
};

class LineEditSettings {
public:
    LineEditSettings() = default;
    explicit LineEditSettings(QRegularExpression input_valid_rgx, QRegularExpression input_invalid_rgx,
                              SqlColumnNum sql_column)
        : _input_valid_rgx(input_valid_rgx), _input_invalid_rgx(input_invalid_rgx),
          _sql_column(sql_column) {}

    const std::tuple<QRegularExpression, QRegularExpression, SqlColumnNum> getAll() const
    {
        return {_input_valid_rgx, _input_invalid_rgx, _sql_column};
    }
private:
    QRegularExpression _input_valid_rgx = QRegularExpression("");
    QRegularExpression _input_invalid_rgx = QRegularExpression("");
    SqlColumnNum _sql_column = SqlColumnNum(-1);
};

namespace Ui {
class NewFlight;
}

class NewFlight : public QDialog
{
    Q_OBJECT

public:
    explicit NewFlight(QWidget *parent);
    explicit NewFlight(QWidget *parent, Flight oldFlight);
    ~NewFlight();

    QStringList* getResult();

private:

    void setup();

    void formFiller(Flight oldFlight);

    void setupLineEdit(QLineEdit* line_edit, LineEditSettings settings);

    void nope();//error box

    void addNewPilotMessageBox();

    void readSettings();

    void writeSettings();

    void collectBasicData();

    void collectAdditionalData();

    void fillExtras();

    bool verifyInput();

    void onInputRejected(QLineEdit* line_edit, QRegularExpression rgx);

    void onEditingFinishedCleanup(QLineEdit* line_edit);

    void update();

    bool isLessOrEqualToTotalTime(QString timeString);

private slots:

    void on_verifyButton_clicked(); //debug button

    void on_deptTZ_currentTextChanged(const QString &arg1);
    void on_destTZ_currentIndexChanged(const QString &arg1);

    void on_tofbTimeLineEdit_editingFinished();
    void on_tonbTimeLineEdit_editingFinished();
    void on_tofbTimeLineEdit_inputRejected();
    void on_tonbTimeLineEdit_inputRejected();

    void on_deptLocLineEdit_editingFinished();
    void on_destLocLineEdit_editingFinished();
    void on_destLocLineEdit_inputRejected();
    void on_deptLocLineEdit_inputRejected();

    void on_deptLocLineEdit_textEdited(const QString &arg1);
    void on_destLocLineEdit_textEdited(const QString &arg1);

    void on_newDoft_editingFinished();

    void on_acftLineEdit_inputRejected();
    void on_acftLineEdit_editingFinished();

    void on_picNameLineEdit_inputRejected();
    void on_picNameLineEdit_editingFinished();
    void on_secondPilotNameLineEdit_editingFinished();
    void on_secondPilotNameLineEdit_inputRejected();
    void on_thirdPilotNameLineEdit_editingFinished();
    void on_thirdPilotNameLineEdit_inputRejected();

    void on_setAsDefaultButton_clicked();
    void on_restoreDefaultButton_clicked();

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_PilotFlyingCheckBox_stateChanged(int);

    void on_ApproachComboBox_currentTextChanged(const QString &arg1);

    void on_tSPSETimeLineEdit_editingFinished();
    void on_tSPMETimeLineEdit_editingFinished();

    void on_tMPTimeLineEdit_editingFinished();

    void on_tIFRTimeLineEdit_editingFinished();
    void on_tNIGHTTimeLineEdit_editingFinished();
    //void on_xcTimeLineEdit_editingFinished();
    void on_tPICTimeLineEdit_editingFinished();
    void on_tSICTimeLineEdit_editingFinished();
    void on_tDualTimeLineEdit_editingFinished();
    void on_tFITimeLineEdit_editingFinished();
    //void on_simTimeLineEdit_editingFinished();
    void on_FlightNumberLineEdit_editingFinished();

signals:
    void mandatoryFieldsValid(NewFlight* nf);

private:
    Ui::NewFlight *ui;
    QMap<QLineEdit*, int> lineEditBitMap;
    QVector<QLineEdit*> mandatoryLineEdits;
    QBitArray allOkBits;
    QMessageBox messageBox;
    QStringList result;
    // For Flight Object
    QMap<QString, QString> airportMap;
    QStringList airports;
    QStringList pilots;
    QStringList tails;
    QMap<QString, QString> newData;
};

#endif // NEWFLIGHT_H
