#include "airportinput.h"
#include "src/database/databasecache.h"
bool AirportInput::isValid() const
{
    return DBCache->getAirportsMapICAO().key(input) != 0;
}

QString AirportInput::fixup() const
{
    QString fixed;

    if(input.length() == 3) {
        //input could be IATA code, try to look up Airport ID and match to ICAO code
        int id = DBCache->getAirportsMapIATA().key(input.toUpper());
        if (id != 0)
            fixed = DBCache->getAirportsMapICAO().value(id);
    } else {
        //input could be lower case
        int id = DBCache->getAirportsMapICAO().key(input.toUpper());
        if (id != 0)
            fixed = input.toUpper();
    }
    return fixed;
}
