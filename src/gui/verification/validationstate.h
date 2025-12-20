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

    /*!
     * \brief ValidationState - The default ValidationState object is initialised with all values false
     */
    ValidationState() = default;

    /*!
     * \brief The ValidationItem enum contains the items that are mandatory for logging a flight:
     * Date of Flight, Departure, Destination, Time Off Blocks, Time On Blocks, Pilot in Command, Aircraft Registration
     */
    enum ValidationItem {DOFT = 0, DEPT = 1, DEST = 2, TOFB = 3, TONB = 4, PIC = 5, ACFT = 6};

    inline void validate(ValidationItem item)             { validationArray[item]  = true;};
    inline void validate(int index)                       { validationArray[index] = true;};
    inline void invalidate(ValidationItem item)           { validationArray[item]  = false;}
    inline void invalidate(int index)                     { validationArray[index] = false;}
    inline bool allValid() const                   { return validationArray.count(true) == 7; }
    inline bool allButOneValid() const			   { return validationArray.count(false) < 2; }
    inline bool timesValid() const                 { return validationArray[ValidationItem::TOFB] && validationArray[ValidationItem::TONB];}
    inline bool locationsValid() const             { return validationArray[ValidationItem::DEPT] && validationArray[ValidationItem::DEST];}
    inline bool nightDataValid() const             { return timesValid() && locationsValid() && validationArray[ValidationItem::DOFT];}
    inline bool acftValid() const                  { return validationArray[ValidationItem::ACFT];}
    inline bool validAt(int index) const           { return validationArray[index];}
    inline bool validAt(ValidationItem item) const { return validationArray[item];}
    inline int validCount() const				   { return validationArray.count(true);}
    inline int invalidCount() const				   { return validationArray.count(false);}
    QStringList invalidItemDisplayNames() const {
        QStringList ret;
        for (auto it = validationItemsDisplayNames.constBegin(); it != validationItemsDisplayNames.constEnd(); ++it) {
            if (!validAt(it.key())) {
                ret.append(it.value());
            }
        }
        return ret;
    }



    // Debug
    void printValidationStatus() const {
        QString deb_string("\033[mValidation State:\n\t\tdoft\tdept\tdest\ttofb\ttonb\tpic\tacft\n\t");
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
    static const inline QMap<ValidationState::ValidationItem, QString> validationItemsDisplayNames = {
        {ValidationState::ValidationItem::DOFT, QObject::tr("Date of Flight")},
        {ValidationState::ValidationItem::DEPT, QObject::tr("Departure Airport")},
        {ValidationState::ValidationItem::DEST, QObject::tr("Destination Airport")},
        {ValidationState::ValidationItem::TOFB, QObject::tr("Time Off Blocks")},
        {ValidationState::ValidationItem::TONB, QObject::tr("Time On Blocks")},
        {ValidationState::ValidationItem::PIC,  QObject::tr("PIC Name")},
        {ValidationState::ValidationItem::ACFT, QObject::tr("Aircraft Registration")},
        };
};
#endif // VALIDATIONSTATE_H
