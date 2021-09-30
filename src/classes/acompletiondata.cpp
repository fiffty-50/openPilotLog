#include "acompletiondata.h"

ACompletionData::ACompletionData()
{
    // retreive user modifiable data
    pilotList   = aDB->getCompletionList(ADatabaseTarget::pilots);
    tailsList   = aDB->getCompletionList(ADatabaseTarget::registrations);
    pilotsIdMap = aDB->getIdMap(ADatabaseTarget::pilots);
    tailsIdMap  = aDB->getIdMap(ADatabaseTarget::tails);

    // retreive default data
    airportIcaoIdMap = aDB->getIdMap(ADatabaseTarget::airport_identifier_icao);
    airportIataIdMap = aDB->getIdMap(ADatabaseTarget::airport_identifier_iata);
    airportNameIdMap = aDB->getIdMap(ADatabaseTarget::airport_names);
    airportList      = aDB->getCompletionList(ADatabaseTarget::airport_identifier_all);

}

void ACompletionData::update()
{
    // retreive user modifiable data
    pilotList   = aDB->getCompletionList(ADatabaseTarget::pilots);
    tailsList   = aDB->getCompletionList(ADatabaseTarget::registrations);
    pilotsIdMap = aDB->getIdMap(ADatabaseTarget::pilots);
    tailsIdMap  = aDB->getIdMap(ADatabaseTarget::tails);
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
