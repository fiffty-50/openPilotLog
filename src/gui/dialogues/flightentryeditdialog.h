#ifndef FLIGHTENTRYEDITDIALOG_H
#define FLIGHTENTRYEDITDIALOG_H

#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCalendarWidget>
#include "entryeditdialog.h"
#include "src/gui/verification/userinput.h"

class FlightEntryEditDialog : public EntryEditDialog
{
public:
    FlightEntryEditDialog(QWidget *parent = nullptr);
    FlightEntryEditDialog(int rowId, QWidget *parent = nullptr);
    virtual void loadEntry(int rowID) override;
    virtual void loadEntry(OPL::Row entry) override;
    virtual bool deleteEntry(int rowID) override;

private:
    OPL::DateTimeFormat dateTimeFormat = OPL::DateTimeFormat();

    QLineEdit dateLineEdit = QLineEdit(this);
    QLineEdit timeOutLineEdit = QLineEdit(this);
    QLineEdit timeOffLineEdit = QLineEdit(this);
    QLineEdit timeOnLineEdit =  QLineEdit(this);
    QLineEdit timeInLineEdit = QLineEdit(this);

    QLineEdit departureLineEdit = QLineEdit(this);
    QLineEdit destinationLineEdit = QLineEdit(this);

    QLineEdit firstPilotLineEdit =  QLineEdit(this);
    QLineEdit secondPilotLineEdit = QLineEdit(this);
    QLineEdit thirdPilotLineEdit = QLineEdit(this);

    QLineEdit registrationLineEdit = QLineEdit(this);

    QLineEdit remarksLineEdit = QLineEdit(this);
    QLineEdit flightNumberLineEdit = QLineEdit(this);

    QComboBox pilotFunctionComboBox = QComboBox(this);
    QComboBox flightRulesComboBox = QComboBox(this);
    QComboBox approachTypeComboBox = QComboBox(this);

    QSpinBox takeOffCountSpinBox = QSpinBox(this);
    QSpinBox landingCountSpinBox = QSpinBox(this);

    QCheckBox pilotFlyingCheckBox = QCheckBox(this);

    const QList<QLineEdit *> timeLineEdits = { &timeOutLineEdit, &timeOffLineEdit, &timeOnLineEdit, &timeInLineEdit };
    const QList<QLineEdit *> pilotNameLineEdits = { &firstPilotLineEdit, &secondPilotLineEdit, &thirdPilotLineEdit };
    const QList<QLineEdit *> mandatoryLineEdits = { &dateLineEdit, &timeOutLineEdit, &timeInLineEdit, &departureLineEdit,
                                                    &destinationLineEdit, &firstPilotLineEdit, &registrationLineEdit };

    QPushButton dateButton = QPushButton(tr("Date"), this);
    QLabel dateDisplayLabel = QLabel(this);
    QLabel timeOutLabel = QLabel(tr("Off Blocks"), this);
    QLabel timeOffLabel = QLabel(tr("Take Off"), this);
    QLabel timeOnLabel = QLabel(tr("Landing"), this);
    QLabel timeInLabel = QLabel(tr("On Blocks"), this);
    QLabel departureLabel = QLabel(tr("Departure"), this);
    QLabel departureDisplayLabel = QLabel(this);
    QLabel destinationLabel = QLabel(tr("Destination"), this);
    QLabel destinationDisplayLabel = QLabel(this);
    QLabel firstPilotLabel = QLabel(tr("PIC"), this);
    QLabel secondPilotLabel = QLabel(tr("SIC"), this);
    QLabel thirdPilotLabel = QLabel(tr("Third Pilot"), this);
    QLabel registrationLabel = QLabel(tr("Registration"), this);
    QLabel remarksLabel = QLabel(tr("Remarks"), this);
    QLabel flightNumberLabel = QLabel(tr("Flight Number"), this);
    QLabel pilotFlyingLabel = QLabel(tr("Pilot Flying"), this);
    QLabel takeOffCountLabel = QLabel(tr("Take Off"), this);
    QLabel landingCountLabel = QLabel(tr("Landing"), this);
    QLabel totalTimeLabel = QLabel(tr("Total Time"), this);
    QLabel totalTimeDisplayLabel = QLabel("00:00", this);
    QLabel pilotFunctionLabel = QLabel(tr("Function"), this);
    QLabel approachTypeLabel = QLabel(tr("Approach"), this);
    QLabel flightRulesLabel = QLabel(tr("Flight Rules"), this);

    QCalendarWidget *calendarWidget;

    QDialogButtonBox acceptButtonBox = QDialogButtonBox(QDialogButtonBox::Ok |
                                                        QDialogButtonBox::Cancel);

    OPL::DateTimeFormat displayFormat;

    void init();
    void setupUI();
    void setupSignalsAndSlots();
    void readSettings();
    bool verifyUserInput(QLineEdit *lineEdit, const UserInput &input);
    inline void setRedBorder(QLineEdit *lineEdit) {
        lineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
    }
    inline void clearBorder(QLineEdit *lineEdit) {
        lineEdit->setStyleSheet(QString());
    }


private slots:
    void onDialogAccepted();
    void onCalendarRequested();
    void onCalendarDateSelected();


    // line edits
    void onDateEditingFinished();
    void onTimeEditingFinished(QLineEdit *lineEdit);
    void onNameEditingFinished();
    void onRegistrationEditingFinished();
    void onLocationEditingFinished();
};

#endif // FLIGHTENTRYEDITDIALOG_H
