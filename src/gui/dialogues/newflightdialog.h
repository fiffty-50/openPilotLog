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
#include <QCalendarWidget>
#include <QTabWidget>
#include <QSqlRelationalTableModel>


#include "src/database/db.h"
#include "src/classes/flight.h"
#include "src/classes/aircraft.h"
#include "src/classes/astrictrxvalidator.h"
#include "src/classes/asettings.h"
#include "src/functions/acalc.h"
#include "src/experimental/adatabase.h"

#include "src/gui/dialogues/newpilotdialog.h"
#include "src/gui/dialogues/newtaildialog.h"

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

class NewFlightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFlightDialog(QWidget *parent, Db::editRole edRole);
    explicit NewFlightDialog(QWidget *parent, Flight oldFlight, Db::editRole edRole);
    explicit NewFlightDialog(QWidget *parent);
    explicit NewFlightDialog(QWidget *parent, int old_flight);
    ~NewFlightDialog();

    //QStringList* getResult();

private:

    bool eventFilter(QObject* object, QEvent* event);

    void setup();

    void formFiller(Flight oldFlight);

    void setupLineEdit(QLineEdit* line_edit, LineEditSettings settings);

    void addNewPilotMessageBox(QLineEdit *parent);

    void addNewAircraftMessageBox(QLineEdit *parent);

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

    void on_doftTimeEdit_editingFinished();

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
    void on_tPICTimeLineEdit_editingFinished();
    void on_tSICTimeLineEdit_editingFinished();
    void on_tDUALTimeLineEdit_editingFinished();
    void on_tFITimeLineEdit_editingFinished();
    void on_FlightNumberLineEdit_textChanged(const QString &arg1);


    void on_manualEditingCheckBox_stateChanged(int arg1);

    void on_FunctionComboBox_currentTextChanged();

    void on_tblkTimeLineEdit_editingFinished();

    void on_IfrCheckBox_stateChanged();

    void on_TakeoffSpinBox_valueChanged(int arg1);

    void on_LandingSpinBox_valueChanged(int arg1);

    void on_AutolandSpinBox_valueChanged(int arg1);

    void on_TakeoffCheckBox_stateChanged(int arg1);

    void on_LandingCheckBox_stateChanged(int arg1);

    void on_AutolandCheckBox_stateChanged(int arg1);

    //void on_doftToolButton_clicked();

    void date_clicked(const QDate &date);

    void date_selected(const QDate &date);

    void on_doftLineEdit_inputRejected();

    void on_doftLineEdit_editingFinished();

    void on_doftLineEditEntered();

signals:
    void mandatoryFieldsValid(NewFlightDialog* nf);

private:
    Db::editRole role;
    Flight entry;
    bool doUpdate;
    Ui::NewFlight *ui;
    QMap<QLineEdit*, int> lineEditBitMap;
    QVector<QLineEdit*> mandatoryLineEdits;
    QBitArray allOkBits;
    QMessageBox messageBox;
    QDate clickedDate;
    // For Flight Object
    QMap<QString, QString> airportMap;
    QStringList airports;
    QStringList pilots;
    QStringList tails;
    QMap<QString, QString> newData;
};

#endif // NEWFLIGHT_H
