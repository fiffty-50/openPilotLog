#ifndef ENTRY_H
#define ENTRY_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>

#include "Decl.h"

namespace experimental {

/*!
 * \brief The Entry class encapsulates table metadata(table name, row id)
 *  and data for new and existing entries in the database to operate on.
 */
class Entry {
public:
    QPair<QString, int> position;
protected:
    QMap<QString, QString> tableData;
public:
    Entry() = default;
    Entry(const Entry&) = default;
    void operator=(const Entry&);
    Entry(QPair<QString, int> position_);
    void setData(QMap<QString, QString> data);
    const QMap<QString, QString>& getData();

};

class PilotEntry : public Entry {
public:
    PilotEntry() = default;
    PilotEntry(const PilotEntry& pe);
    void operator=(const PilotEntry& pe);
    PilotEntry(int row_id);
    PilotEntry(QMap<QString, QString> fromNewPilotDialog);
};

}

#endif // ENTRY_H
