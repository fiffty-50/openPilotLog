#include "completerprovider.h"
#include "src/database/databasecache.h"

//namespace OPL {

CompleterProvider::CompleterProvider()
{
    pilotCompleter    = new QCompleter(DBCache->getPilotNamesList());
    tailsCompleter    = new QCompleter(DBCache->getTailsList());
    airportCompleter  = new QCompleter(DBCache->getAirportList());
    companyCompleter  = new QCompleter(DBCache->getCompaniesList());
    aircraftCompleter = new QCompleter(DBCache->getAircraftList());

    QList<QCompleter*> completers = {
        pilotCompleter,
        tailsCompleter,
        airportCompleter,
        companyCompleter,
        aircraftCompleter,
    };
    for (const auto completer : completers) {
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
    }

    // Listen for changes in Database Cache
    QObject::connect(DBCache,    	&OPL::DatabaseCache::databaseCacheUpdated,
                     this,			&CompleterProvider::onDatabaseCacheUpdated);
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
    QStringListModel* model = nullptr;

    switch(target) {
    case Airports:
        newData = &DBCache->getAirportList();
        model = qobject_cast<QStringListModel*>(airportCompleter->model());
        break;
    case Pilots:
        newData = &DBCache->getPilotNamesList();
        model = qobject_cast<QStringListModel*>(pilotCompleter->model());
        break;
    case Tails: {
        newData = &DBCache->getTailsList();
        model = qobject_cast<QStringListModel*>(tailsCompleter->model());
        break;
    }
    default:
        break;
    }

    if(newData == nullptr) return;

    model->setStringList(*newData);
}

//} // namespace OPL
