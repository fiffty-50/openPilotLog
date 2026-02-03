#include "completerprovider.h"
#include "src/database/databasecache.h"
#include "src/gui/verification/diacriticignoringcompleter.h"

// namespace OPL {

CompleterProvider::CompleterProvider()
{
    // create normal completers
    airportCompleter = new QCompleter(DBCache->getList(OPL::DatabaseCache::ListType::AirportCodes));
    companyCompleter = new QCompleter(DBCache->getList(OPL::DatabaseCache::ListType::Companies));
    aircraftCompleter =
        new QCompleter(DBCache->getList(OPL::DatabaseCache::ListType::AircraftTypes));

    // use custom completers for tails and names with improved matching
    auto tailsList = DBCache->getList(OPL::DatabaseCache::ListType::Tails);
    tailsCompleter = DiacriticIgnoringCompleter::createCompleter(tailsList, this);

    auto pilotList = DBCache->getList(OPL::DatabaseCache::ListType::PilotNames);
    pilotCompleter = DiacriticIgnoringCompleter::createCompleter(pilotList, this);

    const QList<QCompleter *> completers = {
        pilotCompleter, tailsCompleter, airportCompleter, companyCompleter, aircraftCompleter,
    };

    for (const auto &completer : completers) {
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
    }

    // Listen for changes in Database Cache
    QObject::connect(DBCache, &OPL::DatabaseCache::databaseCacheUpdated, this,
                     &CompleterProvider::onDatabaseCacheUpdated);
}

CompleterProvider::~CompleterProvider()
{
    pilotCompleter->deleteLater();
    tailsCompleter->deleteLater();
    airportCompleter->deleteLater();
}

QCompleter *CompleterProvider::getCompleter(CompleterTarget target) const
{
    switch (target) {
    case Airports:
        return airportCompleter;
        break;
    case Pilots:
        return pilotCompleter;
        break;
    case Tails:
        return tailsCompleter;
    case Aircraft:
        return aircraftCompleter;
    case Companies:
        return companyCompleter;
        break;
    default:
        return nullptr;
        break;
    }
}

void CompleterProvider::onDatabaseCacheUpdated(const OPL::DbTable table)
{
    switch (table) {
    case OPL::DbTable::Pilots:
        DEB << "Pilots completer model updated...";
        updateModel(CompleterTarget::Pilots);
        break;
    case OPL::DbTable::Tails:
        DEB << "Tails completer model updated...";
        updateModel(CompleterTarget::Tails);
        break;
    case OPL::DbTable::Airports:
        DEB << "Airports completer model updated...";
        updateModel(CompleterTarget::Airports);
        break;
    default:
        break;
    }
}

void CompleterProvider::updateModel(CompleterTarget target)
{
    const QStringList *newData = nullptr;
    QStringListModel *model    = nullptr;

    switch (target) {
    case Airports:
        newData = &DBCache->getList(OPL::DatabaseCache::ListType::AirportCodes);
        model   = qobject_cast<QStringListModel *>(airportCompleter->model());
        break;
    case Pilots:
        newData = &DBCache->getList(OPL::DatabaseCache::ListType::PilotNames);
        model   = qobject_cast<QStringListModel *>(pilotCompleter->model());
        break;
    case Tails: {
        newData = &DBCache->getList(OPL::DatabaseCache::ListType::Tails);
        model   = qobject_cast<QStringListModel *>(tailsCompleter->model());
        break;
    }
    default:
        break;
    }

    if (newData == nullptr) return;
    DEB << "New data:" << *newData;
    model->setStringList(*newData);
}

//} // namespace OPL
