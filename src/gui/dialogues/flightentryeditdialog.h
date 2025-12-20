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
#include "src/gui/verification/validationstate.h"
#include "src/opl.h"
#include "src/database/flightentry.h"
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
    OPL::DateTimeFormat m_displayFormat;
    OPL::FlightEntry m_flightEntry;

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
    const QList<QLineEdit *> locationLineEdits = { &departureLineEdit, &destinationLineEdit};
    const QList<QLineEdit *> pilotNameLineEdits = { &firstPilotLineEdit, &secondPilotLineEdit, &thirdPilotLineEdit };
    // const QHash<QLineEdit *,  ValidationState::ValidationItem> mandatoryLineEdits = {
    //                                                                                        {&dateLineEdit, 		   ValidationState::DOFT},
    //                                                                                        {&departureLineEdit,    ValidationState::DEPT},
    //                                                                                        {&destinationLineEdit,  ValidationState::DEST},
    //                                                                                        {&timeOutLineEdit, 	   ValidationState::TOFB },
    //                                                                                        {&timeInLineEdit, 	   ValidationState::TONB },
    //                                                                                        {&firstPilotLineEdit,   ValidationState::PIC },
    //                                                                                        {&registrationLineEdit, ValidationState::ACFT },
    //     };
    // const QVector<QLineEdit *> mandatoryLineEdits = { &dateLineEdit, &departureLineEdit,
    //                                                   &destinationLineEdit, &timeOutLineEdit,
    //                                                   &timeInLineEdit,  &firstPilotLineEdit, &registrationLineEdit };

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

    /*!
     * \brief pilotFuncionsMap Maps the function times to its index in the pilotFunctionComboBox
     */
    static const inline QHash<int, QString> pilotFuncionsMap = {
                                                                {0, OPL::FlightEntry::TPIC},
                                                                {1, OPL::FlightEntry::TPICUS},
                                                                {2, OPL::FlightEntry::TSIC},
                                                                {3, OPL::FlightEntry::TDUAL},
                                                                {4, OPL::FlightEntry::TFI},
                                                                };
    void init();
    void setupUI();
    void setupAutoCompletion();
    void setupSignalsAndSlots();
    void readSettings();
    bool verifyUserInput(QLineEdit *lineEdit, const UserInput &input);
    void onBadInputReceived(QLineEdit *lineEdit);
    void onGoodInputReceived(QLineEdit *lineEdit);

    /*!
     * \brief Add the data from combo and spin boxes to the flight entry
     */
    void collectSecondaryFlightData();


private slots:
    void onDialogAccepted();
    void onCalendarRequested();
    void onCalendarDateSelected();


    // line edits
    void onDateEditingFinished();
    void onTimeOutEditingFinished();
    void onTimeInEditingFinished();
    void onNameEditingFinished(QLineEdit *lineEdit);
    void onRegistrationEditingFinished(QLineEdit *lineEdit);
    void onLocationEditingFinished(QLineEdit *lineEdit);

    void onRemarksEditingFinished();
    void onFlightNumberEditingFinished();

    /*!
     * \brief adds a new Database Element
     * \param caller - The Line Edit that called the function
     * \param table - The OPL::DBTable where the entry is to be created
     * \return true if a new entry was succesfully created
     * \details This function queries the user if he wants to add a new Element to the
     * Database. If so desired, a new dialog is launched to create the entry. If the
     * return code indicates success, the caller text is set to the just now created
     * new entry
     */
    bool addNewDatabaseElement(QLineEdit *caller, const OPL::DbTable table);

protected:
    /*!
    * \brief invalidates mandatory line edits on focus in.
    * \details Some of the QLineEdits have validators set that provide raw input validation. These validators have the side effect
    * that if an input does not meet the raw input validation criteria, onEditingFinished() is not emitted when the line edit loses
    * focus. This could lead to a line edit that previously had good input to be changed to bad input without the validation bit
    * in validationState being unset, because the second step of input validation is only triggered when editingFinished() is emitted.
    *
    * This event filter invalidates the validation state on focus in events for a mandatory line edit
    */
    // bool eventFilter(QObject *object, QEvent *event) override {
    //     const auto lineEdit = qobject_cast<QLineEdit*>(object);
    //     if (mandatoryLineEdits.contains(lineEdit) && event->type() == QEvent::FocusIn) {
    //         // set verification bit to false when entering a mandatory line edit
    //         validationState.invalidate(mandatoryLineEdits.value(lineEdit));
    //         return false;
    //     }

    //     return QObject::eventFilter(object, event);
    //     }
};

#endif // FLIGHTENTRYEDITDIALOG_H
