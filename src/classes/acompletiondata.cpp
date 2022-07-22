#include "acompletiondata.h"

void ACompletionData::init()
{
    // retreive user modifiable data
    pilotsIdMap = aDB->getIdMap(DatabaseTarget::PilotNames);
    tailsIdMap  = aDB->getIdMap(DatabaseTarget::Registrations);
    pilotList   = aDB->getCompletionList(DatabaseTarget::PilotNames);
    tailsList   = aDB->getCompletionList(DatabaseTarget::Registrations);

    // For tails, also provide completion for registration stripped of the '-' character
    QStringList tails_list = aDB->getCompletionList(DatabaseTarget::Registrations);
    for (auto &reg : tails_list) {
        if(reg.contains(QLatin1Char('-'))) { // check to avoid duplication if reg has no '-'
            QString copy = reg;
            reg.remove(QLatin1Char('-'));
            reg = copy + " (" + reg + QLatin1Char(')');
        }
    }
    tailsList   = tails_list;

    // retreive default data
    airportIcaoIdMap = aDB->getIdMap(DatabaseTarget::AirportIdentifierICAO);
    airportIataIdMap = aDB->getIdMap(DatabaseTarget::AirportIdentifierIATA);
    airportNameIdMap = aDB->getIdMap(DatabaseTarget::AirportNames);
    airportList      = aDB->getCompletionList(DatabaseTarget::AirportIdentifier);

    current_state = aDB->getUserDataState();
}

void ACompletionData::update()
{
    if (current_state != aDB->getUserDataState()) {
        // retreive user modifiable data
        pilotList   = aDB->getCompletionList(DatabaseTarget::PilotNames);
        tailsList   = aDB->getCompletionList(DatabaseTarget::Registrations);
        pilotsIdMap = aDB->getIdMap(DatabaseTarget::PilotNames);
        tailsIdMap  = aDB->getIdMap(DatabaseTarget::Registrations);

        current_state = aDB->getUserDataState();
    }
}

void ACompletionData::updateTails()
{
    tailsIdMap  = aDB->getIdMap(DatabaseTarget::Registrations);
    tailsList   = aDB->getCompletionList(DatabaseTarget::Registrations);
}

void ACompletionData::updatePilots()
{
    pilotsIdMap  = aDB->getIdMap(DatabaseTarget::PilotNames);
    pilotList    = aDB->getCompletionList(DatabaseTarget::PilotNames);
}
