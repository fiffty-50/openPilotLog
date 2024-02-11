#ifndef VALIDATIONSTATE_H
#define VALIDATIONSTATE_H
#include <QtCore>

/*!
 * \brief Holds information about whether mandatory components of a new flight entry have been validated.
 * \details The ValidationState class encapsulates a QBitArray that has a bit set (or unset) depending on wether the
 * input for the associated index has been verified. The indexes correspond to the mandatory items enumerated in the
 * ValidationItem enum. It is used by the NewFlightDialog to keep track of validation states.
 */
class ValidationState {
public:
    ValidationState() = default;

    /*!
     * \brief The ValidationItem enum contains the items that are mandatory for logging a flight:
     * Date of Flight, Departure, Destination, Time Off Blocks, Time On Blocks, Pilot in Command, Aircraft Registration
     */
    enum ValidationItem {doft = 0, dept = 1, dest = 2, tofb = 3, tonb = 4, pic = 5, acft = 6};

    void validate(ValidationItem item)             { validationArray[item]  = true;};
    void validate(int index)                       { validationArray[index] = true;};
    void invalidate(ValidationItem item)           { validationArray[item]  = false;}
    void invalidate(int index)                     { validationArray[index] = false;}
    inline bool allValid() const                   { return validationArray.count(true) == 7; }
    inline bool allButOneValid() const			   { return validationArray.count(false) < 2; }
    inline bool timesValid() const                 { return validationArray[ValidationItem::tofb] && validationArray[ValidationItem::tonb];}
    inline bool locationsValid() const             { return validationArray[ValidationItem::dept] && validationArray[ValidationItem::dest];}
    inline bool nightDataValid() const             { return timesValid() && locationsValid() && validationArray[ValidationItem::doft];}
    inline bool acftValid() const                  { return validationArray[ValidationItem::acft];}
    inline bool validAt(int index) const           { return validationArray[index];}
    inline bool validAt(ValidationItem item) const { return validationArray[item];}

    // Debug
    void printValidationStatus() const {
        QString deb_string("\033[mValidation State:\tdoft\tdept\tdest\ttofb\ttonb\tpic\tacft\n");
        deb_string += "\t\t\t\t";
        for (int i = 0; i < 7; i++) { //\033[32m
            if (validationArray[i])
                deb_string += "\t\033[32m" + QString::number(validationArray[i]);
            else
                deb_string += "\t\033[31m" + QString::number(validationArray[i]);
        }
        deb_string += QLatin1String("\u001b[38;5;75m"); // return to default DEB
        qDebug().noquote() << deb_string;
    }
private:
    QBitArray validationArray = QBitArray(7);
};
#endif // VALIDATIONSTATE_H
