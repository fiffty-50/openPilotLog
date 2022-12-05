#include "completerprovider.h"
#include "src/database/databasecache.h"

//namespace OPL {

CompleterProvider::CompleterProvider()
{
    pilotCompleter = new QCompleter(OPL::DBCache.getPilotNamesList());
    tailsCompleter = new QCompleter(OPL::DBCache.getTailsList());
    airportCompleter = new QCompleter(OPL::DBCache.getAirportList());

    QList<QCompleter*> completers = {
        pilotCompleter,
        tailsCompleter,
        airportCompleter,
    };
    for (const auto completer : completers) {
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
    }
}

CompleterProvider::~CompleterProvider()
{
    delete pilotCompleter;
    delete tailsCompleter;
    delete airportCompleter;
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
        break;
    default:
        break;
    }
}

/*
QCompleter *ValidatorProvider::newCompleter(CompleterTarget target, QObject *parent)
{
    QCompleter* completer = nullptr;
    switch(target) {
    case Airports:
        completer = new QCompleter(OPL::DBCACHE.getAirportList(), parent);
        break;
    case Pilots:
        completer = new QCompleter(OPL::DBCACHE.getPilotNamesList(), parent);
        break;
    case Tails: {
        completer = new QCompleter(OPL::DBCACHE.getTailsList(), parent);
        break;
    }
    default:
        return nullptr;
    }

    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);

    return completer;
}
*/
void CompleterProvider::updateModel(CompleterTarget target)
{
    const QStringList *newData = nullptr;
    QStringListModel* model = nullptr;

    switch(target) {
    case Airports:
        newData = &OPL::DBCache.getAirportList();
        model = qobject_cast<QStringListModel*>(airportCompleter->model());
        break;
    case Pilots:
        newData = &OPL::DBCache.getPilotNamesList();
        model = qobject_cast<QStringListModel*>(pilotCompleter->model());
        break;
    case Tails: {
        newData = &OPL::DBCache.getTailsList();
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
