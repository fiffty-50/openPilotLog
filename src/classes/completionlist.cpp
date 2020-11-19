/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "completionlist.h"

CompletionList::CompletionList()
{

}

/*!
 * \brief completionList::completionList construcs a completionList object.
 * Access object->list for the list.
 * \param type see enum completerTarget::targets
 */
CompletionList::CompletionList(CompleterTarget::targets type)
{
    QString query;
    QVector<QString> columns;
    QVector<QString> result;

    switch (type) {
    case CompleterTarget::airports:
        columns.append("icao");
        columns.append("iata");
        result = Db::multiSelect(columns, "airports");
        break;
    case CompleterTarget::registrations:
        columns.append("registration");
        result = Db::multiSelect(columns, "tails");
        break;
    case CompleterTarget::companies:
        columns.append("company");
        result = Db::multiSelect(columns, "pilots");
        break;
    case CompleterTarget::pilots:
        query.append("SELECT piclastname||','||picfirstname FROM pilots");
        result = Db::customQuery(query, 1);
        break;
    case CompleterTarget::aircraft:
        query.append("SELECT make||' '||model||'-'||variant FROM aircraft");
        result = Db::customQuery(query, 1);
        break;
    }

    list = result.toList();
    list.removeAll(QString(""));
    list.removeDuplicates();
}
