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
#include "aircraft.h"

// Debug Makro
#define DEB(expr) \
    qDebug() << "aircraft ::" << __func__ << "\t" << expr

aircraft::aircraft()
{

}

/*!
 * \brief aircraft::aircraft creates an aircraft object from the database.
 * \param database_id primary key in database
 * \param db either database::tails or database::acft
 */
aircraft::aircraft(int database_id, aircraft::database table)
{
    QVector<QString> columns = {
        "tail_id", "registration", "company",
        "make", "model", "variant",
        "singlepilot", "multipilot", "singleengine", "multiengine",
        "unpowered", "piston", "turboprop",
        "jet", "light", "medium", "heavy", "super"
    };
    QString checkColumn;
    QString tableName;

    switch (table) {
    case database::tail:
        //DEB("tails:" << columns);
        tableName.append("tails");
        checkColumn.append("tail_id");
        break;
    case database::acft:
        columns.replace(0,"aircraft_id");
        columns.remove(1,2);
        //DEB("acft:" << columns;)
        tableName.append("aircraft");
        checkColumn.append("aircraft_id");
        break;
    }

    auto vector = db::multiSelect(columns,tableName,checkColumn,QString::number(database_id),sql::exactMatch);

    if(vector.length() < 2){
        id = "invalid";
    }else{
        switch (table) {
        case database::tail:
            id = vector[0];
            registration = vector[1];
            company = vector[2];
            make = vector[3];
            model = vector[4];
            variant = vector[5];
            //bool
            singlepilot = vector[6].toInt();
            multipilot = vector[7].toInt();
            singleengine = vector[8].toInt();
            multiengine = vector[9].toInt();
            unpowered = vector[10].toInt();
            piston = vector[11].toInt();
            turboprop = vector[12].toInt();
            jet = vector[13].toInt();
            light = vector[14].toInt();
            medium = vector[15].toInt();
            heavy = vector[16].toInt();
            super = vector[17].toInt();
            break;
        case database::acft:
            id = vector[0];
            make = vector[1];
            model = vector[2];
            variant = vector[3];
            //bool
            singlepilot = vector[4].toInt();
            multipilot = vector[5].toInt();
            singleengine = vector[6].toInt();
            multiengine = vector[7].toInt();
            unpowered = vector[8].toInt();
            piston = vector[9].toInt();
            turboprop = vector[10].toInt();
            jet = vector[11].toInt();
            light = vector[12].toInt();
            medium = vector[13].toInt();
            heavy = vector[14].toInt();
            super = vector[15].toInt();
            break;
        }
    }
}

/*!
 * \brief aircraft::print Debug output
 */
void aircraft::print()
{
    QTextStream cout(stdout, QIODevice::WriteOnly);

    cout << "\t\033[38;2;0;255;0;48;2;0;0;0m Aircraft Object \033[0m\n";
    cout << "ID: \t\t" << id << "\n";
    cout << "Reg: \t\t" << registration<< "\n";
    cout << "Company: \t" << company<< "\n";
    cout << "Type:\t\t" << make << " " << model << " " <<  variant<< "\n";
    cout << "SP:\t\t" << singlepilot << "\tMP:\t" << multipilot << "\t"
         << "SE:\t" << singleengine << "\tME:\t" << multiengine << "\n";
    cout << "UNP:\t\t" << unpowered << "\tPIS:\t" << piston << "\t"
         << "TPR:\t" << turboprop << "\tJET:\t" << jet << "\n";
    cout << "Light:\t" << light << "\tMedium:\t" << medium;
    cout << "Heavy:\t" << heavy << "\tSuper:\t" << super << "\n";

}

QString aircraft::debug()
{
    print();
    return QString();
}
