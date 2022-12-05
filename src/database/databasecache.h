#ifndef DATABASECACHE_H
#define DATABASECACHE_H
#include "QtWidgets/qcompleter.h"
#include "src/opl.h"
#include <QtCore>

namespace OPL{

using IdMap = QHash<int, QString>;
#define DBCache DatabaseCache::getInstance()

/*!
 * \brief Caches certain often accessed database content in memory
 * \details Access to the database is rather slow and memory these days is cheap enough to cache some contents for ease
 * and speed of access. This class provides lists of database entries that are used as inputs for QCompleter instances
 * as well as maps which contain database entries and their associated row ids, which are used for user input verification.
 *
 * The Cache can be accessed by using the DBCACHE macro and needs to be updated whenever the database contents are modified.
 */
class DatabaseCache : public QObject
{
public:
    static DatabaseCache& getInstance() {
        static DatabaseCache instance;
        return instance;
    }

    DatabaseCache(DatabaseCache const&) = delete;
    void operator=(DatabaseCache const&) = delete;

    enum CompleterTarget {PilotNames, Tails, AircraftTypes, Airports, AirportsICAO, AirportsIATA, Companies};

    void init();

    const IdMap &getAirportsMapICAO() const;
    const IdMap &getAirportsMapIATA() const;
    const IdMap &getPilotNamesMap() const;

    const QStringList &getPilotNamesList() const;
    const QStringList &getTailsList() const;
    const QStringList &getAirportList() const;
    const QStringList &getCompaniesList() const;

private:
    Q_OBJECT
    DatabaseCache() {};

    // Id Maps
    IdMap airportsMapICAO;
    IdMap airportsMapIATA;
    IdMap pilotNamesMap;
    IdMap tailsMap;
    // Lists
    QStringList pilotNamesList;
    QStringList tailsList;
    QStringList airportList;
    QStringList companiesList;

    const IdMap fetchMap(CompleterTarget target);
    const QStringList fetchList(CompleterTarget target);


    void updateTails();
    void updateAirports();
    void updateSimulators();
    void updatePilots();

public slots:
    void update(const OPL::DbTable table);

};

}// namespace OPL
#endif // DATABASECACHE_H
