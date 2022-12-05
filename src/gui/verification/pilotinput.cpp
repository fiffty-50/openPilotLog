#include "pilotinput.h"
#include "src/database/databasecache.h"
#include "src/gui/verification/completerprovider.h"

bool PilotInput::isValid() const
{
    return OPL::DBCache.getPilotNamesMap().key(input) != 0;
}

QString PilotInput::fixup() const
{
    if (input.contains(self, Qt::CaseInsensitive))
        return OPL::DBCache.getPilotNamesMap().value(1);

    QCompleter* completer = CompleterProvider::getInstance().getCompleter(CompleterProvider::Pilots);
    completer->setCompletionPrefix(input);
    return completer->currentCompletion();
}
