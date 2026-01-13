#include "tailinput.h"
#include "src/database/databasecache.h"
#include "src/gui/verification/completerprovider.h"

bool TailInput::isValid() const
{
    return DBCache->getKeyMap(OPL::DatabaseCache::MapType::TailRegistrations).value(input) != 0;
}

QString TailInput::fixup() const
{
    QCompleter *completer = QCompleterProvider.getCompleter(CompleterProvider::Tails);
    completer->setCompletionPrefix(input);
    return completer->currentCompletion();
}
