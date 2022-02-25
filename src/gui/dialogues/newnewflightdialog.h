#ifndef NEWNEWFLIGHTDIALOG_H
#define NEWNEWFLIGHTDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QLineEdit>
#include <QList>
#include <QBitArray>

#include "src/functions/atime.h"
#include "src/classes/acompletiondata.h"
#include "src/classes/aflightentry.h"
#include "src/classes/apilotentry.h"
#include "src/classes/atailentry.h"
#include "src/database/adatabase.h"

/*!
 * \brief The ValidationItem enum contains the items that are mandatory for logging a flight:
 * Date of Flight, Departure, Destination, Time Off Blocks, Time On Blocks, Pilot in Command, Aircraft Registration
 */
enum ValidationItem {doft, dept, dest, tofb, tonb, pic, acft};

/*!
 * \brief The ValidationState class encapsulates a QBitArray that has a bit set (or unset) depending on wether the
 * input for the associated index has been verified. The indexes correspond to the mandatory items enumerated in the
 * ValidationItem enum.
 */
class ValidationState {
public:
    ValidationState() = default;

    void validate(ValidationItem line_edit)   { validationArray[line_edit] = true;};
    void invalidate(ValidationItem line_edit) { validationArray[line_edit] = false;}
    bool allValid()                           { return validationArray.count(true) == 6;};
    bool timesValid()                         { return validationArray[ValidationItem::tofb] && validationArray[ValidationItem::tonb];}
    bool locationsValid()                     { return validationArray[ValidationItem::dept] && validationArray[ValidationItem::dest];}
    bool acftValid()                          { return validationArray[ValidationItem::acft];}

    // Debug
    void printValidationStatus(){
        QString deb_string("Validation State:\tdoft\tdept\tdest\ttofb\ttonb\tpic\tacft\n");
        deb_string += "\t\t\t\t\t" + QString::number(validationArray[0]);
        deb_string += "\t" + QString::number(validationArray[1]);
        deb_string += "\t" + QString::number(validationArray[2]);
        deb_string += "\t" + QString::number(validationArray[3]);
        deb_string += "\t" + QString::number(validationArray[4]);
        deb_string += "\t" + QString::number(validationArray[5]);
        deb_string += "\t" + QString::number(validationArray[6]);
        qDebug().noquote() << deb_string;
    }
private:
    QBitArray validationArray = QBitArray(7);
};

/*!
 * \brief The ValidationSetupData struct encapsulates the items required to initialise
 * the line edits with QValidators and QCompleters
 */
struct ValidationSetupData
{
    ValidationSetupData(const QStringList& completion_data, const QRegularExpression& validation_RegEx)
        : completionList(completion_data), validationRegEx(validation_RegEx){};

    ValidationSetupData(const QStringList& completion_data)
        : completionList(completion_data){
    };

    ValidationSetupData(const QRegularExpression& validation_RegEx)
        : completionList(nullptr), validationRegEx(validation_RegEx){};

    const QStringList completionList;
    const QRegularExpression validationRegEx;
};

namespace Ui {
class NewNewFlightDialog;
}

class NewNewFlightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewNewFlightDialog(ACompletionData& completion_data, QWidget *parent = nullptr);
    ~NewNewFlightDialog();

private:
    Ui::NewNewFlightDialog *ui;
    ACompletionData completionData;
    ValidationState validationState;

    static const inline QList<QLineEdit*>* timeLineEdits;
    static const inline QList<QLineEdit*>* locationLineEdits;
    static const inline QList<QLineEdit*>* pilotNameLineEdits;
    static const inline QLatin1String self = QLatin1String("self");

    void init();
    void setupRawInputValidation();
    void setupSignalsAndSlots();
    void readSettings();

};

#endif // NEWNEWFLIGHTDIALOG_H
