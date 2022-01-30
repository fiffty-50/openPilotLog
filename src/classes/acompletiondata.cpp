#include "acompletiondata.h"

void ACompletionData::init()
{
    // retreive user modifiable data
    pilotsIdMap = aDB->getIdMap(ADatabaseTarget::pilots);
    tailsIdMap  = aDB->getIdMap(ADatabaseTarget::tails);
    pilotList   = aDB->getCompletionList(ADatabaseTarget::pilots);
    tailsList   = aDB->getCompletionList(ADatabaseTarget::registrations);

    // For tails, also provide completion for registration stripped of the '-' character
    QStringList tails_list = aDB->getCompletionList(ADatabaseTarget::registrations);
    for (auto &reg : tails_list) {
        if(reg.contains(QLatin1Char('-'))) { // check to avoid duplication if reg has no '-'
            QString copy = reg;
            reg.remove(QLatin1Char('-'));
            reg = copy + " (" + reg + QLatin1Char(')');
        }
    }
    tailsList   = tails_list;

    // retreive default data
    airportIcaoIdMap = aDB->getIdMap(ADatabaseTarget::airport_identifier_icao);
    airportIataIdMap = aDB->getIdMap(ADatabaseTarget::airport_identifier_iata);
    airportNameIdMap = aDB->getIdMap(ADatabaseTarget::airport_names);
    airportList      = aDB->getCompletionList(ADatabaseTarget::airport_identifier_all);

    current_state = aDB->getUserDataState();
}

void ACompletionData::update()
{
    if (current_state != aDB->getUserDataState()) {
        // retreive user modifiable data
        pilotList   = aDB->getCompletionList(ADatabaseTarget::pilots);
        tailsList   = aDB->getCompletionList(ADatabaseTarget::registrations);
        pilotsIdMap = aDB->getIdMap(ADatabaseTarget::pilots);
        tailsIdMap  = aDB->getIdMap(ADatabaseTarget::tails);

        current_state = aDB->getUserDataState();
    }
}

void ACompletionData::updateTails()
{
    tailsIdMap  = aDB->getIdMap(ADatabaseTarget::tails);
    tailsList   = aDB->getCompletionList(ADatabaseTarget::registrations);
}

void ACompletionData::updatePilots()
{
    pilotsIdMap  = aDB->getIdMap(ADatabaseTarget::pilots);
    pilotList    = aDB->getCompletionList(ADatabaseTarget::pilots);
}
