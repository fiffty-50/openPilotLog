#ifndef __USERINPUT_H__
#define __USERINPUT_H__

#include <QPair>
#include <QString>
#include <QBitArray>
#include <QMap>
#include <QStringList>
#include <algorithm>

namespace experimental {

using EntryData = QMap<QString, QString>;

/*!
 * \brief The EntryData struct. Contains ALL possible data.
 * However depending on who is constructing it, different types are initialised.
 * FUTURE: Would it be necessary to able to change data?
 * 	 George: I would say no because we dont want to fuck around with the entry.
 * Collect data from user -> Pack it up in the entry -> Consume it.
 * \todo Figure out exactly what the database would prefer as return value
 *   George: I would assume key: value pairs where the keys are what you would
 * put in the queries. This will affect data aswell (and propably simplify it)
 */
class UserInput {
private:
    const EntryData data;
public:
    const enum class MetaTag {Pilot, Flight, Aircraft} meta_tag;

public:
    UserInput() = delete;
    explicit
    UserInput(EntryData new_data, MetaTag tag)
        :  data(new_data), meta_tag(tag) {}

    QString only(QString data_type) const { return data.value(data_type); }
    const EntryData& all() const { return data; }
};

UserInput newPilotInput(EntryData ed) { return UserInput(ed, UserInput::MetaTag::Pilot); }
UserInput newFlightInput(EntryData ed) { return UserInput(ed, UserInput::MetaTag::Flight); }
UserInput newAircraftInput(EntryData ed) { return UserInput(ed, UserInput::MetaTag::Aircraft); }

}

#endif
