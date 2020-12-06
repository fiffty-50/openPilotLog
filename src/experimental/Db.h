#ifndef __DB_H__
#define __DB_H__

#include <QPair>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include "debug.h"

#include "src/experimental/UserInput.h"

namespace experimental {

/*!
 * \brief findPos: Query database and find table and position of data in uin
 */
static
QPair<QString, int> findPos(UserInput& uin)
{
    switch(uin.meta_tag){
    case UserInput::MetaTag::Pilot:
//        break;
    case UserInput::MetaTag::Flight:
//        break;
    case UserInput::MetaTag::Aircraft:
//        break;
    default:
        return {"", 0};
    }
}

static
bool insertPilot(UserInput& uin)
{
    DEB("Inserting...");
    auto data = uin.all();
    auto position = findPos(uin);

    if (data.isEmpty()) {
        DEB("Object Contains no data. Aborting.");
        return false;
    }
    QString statement = "INSERT INTO " + position.first + QLatin1String(" (");
    QMap<QString, QString>::const_iterator i;
    for (i = data.cbegin(); i != data.cend(); ++i) {
        statement += i.key() + QLatin1String(", ");
    }
    statement.chop(2);
    statement += QLatin1String(") VALUES (");
    for (i = data.cbegin(); i != data.cend(); ++i) {
        statement += QLatin1String("'") + i.value() + QLatin1String("', ");
    }
    statement.chop(2);
    statement += QLatin1String(")");

    QSqlQuery q(statement);
    if (q.lastError().type() == QSqlError::NoError) {
        DEB("Entry successfully committed.");
        return true;
    } else {
        DEB("Unable to commit. Query Error: " << q.lastError().text());
        return false;
    }
}

namespace DB {
    bool init();
    bool commit(UserInput uin)
    {
        switch (uin.meta_tag)
        {
        case UserInput::MetaTag::Pilot:
            insertPilot(uin);
            break;
        case UserInput::MetaTag::Flight:
            // Flight commit logic
            break;
        case UserInput::MetaTag::Aircraft:
            // Aircraft commit logic
            break;
        default:
            return false;
        }
        return true;
    }
    bool remove(UserInput uin) { return false; }
    bool exists(UserInput uin) { return false; }
    bool update(UserInput uin) { return false; }
}

}

#endif
