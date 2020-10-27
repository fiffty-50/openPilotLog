#include "completionlist.h"

completionList::completionList()
{

}

/*!
 * \brief completionList::completionList construcs a completionList object.
 * Access object->list for the list.
 * \param type see enum completerTarget::targets
 */
completionList::completionList(completerTarget::targets type)
{
    QString query;
    QVector<QString> columns;
    QVector<QString> result;

    switch (type) {
    case completerTarget::pilots:
        query.append("SELECT piclastname||', '||picfirstname FROM pilots");
        result = db::customQuery(query,1);
        break;
    case completerTarget::airports:
        columns.append("icao");
        columns.append("iata");
        result = db::multiSelect(columns, "airports");
        break;
    case completerTarget::registrations:
        columns.append("registration");
        result = db::multiSelect(columns, "tails");
        break;
    case completerTarget::aircraft:
        query.append("SELECT make||' '||model||'-'||variant FROM aircraft");
        result = db::customQuery(query,1);
        break;
    }

    completionList::list = result.toList();
    completionList::list.removeAll(QString(""));
}
