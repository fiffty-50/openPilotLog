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
        result = db::customQuery(query, 1);
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
        result = db::customQuery(query, 1);
        break;
    }

    completionList::list = result.toList();
    completionList::list.removeAll(QString(""));
}
