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
#include "entry.h"
// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

Entry::Entry()
{

}

Entry::Entry(QString table, int row)
{
    //retreive database layout
    const auto dbContent = DbInfo();

    if (dbContent.tables.contains(table)) {
        position.first = table;
        columns = dbContent.format.value(table);
    } else {
        DEB(table << " not a table in database. Unable to create Entry object.");
        position.first = QString();
    }

    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + table + " WHERE _rowid_=" + QString::number(row);
    QSqlQuery q(statement);
    q.next();
    int rows = q.value(0).toInt();
    if (rows == 0) {
        DEB("No Entry found for row id: " << row );
        position.second = 0;
    } else {
        DEB("Retreiving data for row id: " << row);
        QString statement = "SELECT * FROM " + table + " WHERE _rowid_=" + QString::number(row);

        QSqlQuery q(statement);
        q.exec();
        q.next();
        for (int i = 0; i < dbContent.format.value(table).length(); i++) {
            data.insert(dbContent.format.value(table)[i], q.value(i).toString());
        }

        error = q.lastError().text();
        if (error.length() > 2) {
            DEB("Error: " << q.lastError().text());
            position.second = 0;
        } else {
            position.second = row;
        }
    }
}

Entry::Entry(QString table, QMap<QString, QString> newData)
{
    //retreive database layout
    const auto dbContent = DbInfo();

    if (dbContent.tables.contains(table)) {
        position.first = table;
        position.second = 0;
        columns = dbContent.format.value(table);
    } else {
        DEB(table << " not a table in database. Unable to create Entry object.");
        position.first = QString();
    }
    //Check validity of newData
    QVector<QString> badkeys;
    QMap<QString, QString>::iterator i;
    for (i = newData.begin(); i != newData.end(); ++i) {
        if (!columns.contains(i.key())) {
            DEB(i.key() << "Not in column list for table " << table << ". Discarding.");
            badkeys << i.key();
        }
    }
    for (const auto &var : badkeys) {
        newData.remove(var);
    }
    data = newData;
}

void Entry::setData(QMap<QString, QString> &value)
{
    //retreive database layout
    const auto dbContent = DbInfo();
    columns = dbContent.format.value(position.first);

    //Check validity of newData
    QVector<QString> badkeys;
    QMap<QString, QString>::iterator i;
    for (i = value.begin(); i != value.end(); ++i) {
        if (!columns.contains(i.key())) {
            DEB(i.key() << "Not in column list for table " << position.first << ". Discarding.");
            badkeys << i.key();
        }
    }
    for (const auto &var : badkeys) {
        value.remove(var);
    }
    data = value;
}

bool Entry::commit()
{
    if (exists()) {
        return update();
    } else {
        return insert();
    }
}

bool Entry::remove()
{
    if (exists()) {
        QString statement = "DELETE FROM " + position.first +
                            " WHERE _rowid_=" + QString::number(position.second);
        QSqlQuery q(statement);
        error = q.lastError().text();

        if (error.length() > 1) {
            DEB("Errors have occured: " << error);
            return false;
        } else {
            DEB("Entry removed.");
            return true;
        }
    } else {
        return false;
    }
}

bool Entry::exists()
{
    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + position.first +
                        " WHERE _rowid_=" + QString::number(position.second);
    QSqlQuery q(statement);
    q.next();
    int rows = q.value(0).toInt();
    if (rows) {
        DEB("Entry exists. " << rows);
        return true;
    } else {
        DEB("Entry does not exist. " << rows);
        return false;
    }
}

bool Entry::insert()
{
    DEB("Inserting...");
    //check prerequisites

    if (data.isEmpty()) {
        DEB("Object Contains no data. Aborting.");
        return false;
    }
    QString statement = "INSERT INTO " + position.first + QLatin1String(" (");
    QMap<QString, QString>::iterator i;
    for (i = data.begin(); i != data.end(); ++i) {
        statement += i.key() + QLatin1String(", ");
    }
    statement.chop(2);
    statement += QLatin1String(") VALUES (");
    for (i = data.begin(); i != data.end(); ++i) {
        statement += QLatin1String("'") + i.value() + QLatin1String("', ");
    }
    statement.chop(2);
    statement += QLatin1String(")");

    QSqlQuery q(statement);
    error = q.lastError().text();
    if (error.length() < 2) {
        DEB("Entry successfully committed.");
        return true;
    } else {
        DEB("Unable to commit. Query Error: " << q.lastError().text());
        return false;
    }
}

bool Entry::update()
{
    //create query
    QString statement = "UPDATE " + position.first + " SET ";

    QMap<QString, QString>::const_iterator i;
    for (i = data.constBegin(); i != data.constEnd(); ++i) {
        if (i.key() != QString()) {
            statement += i.key() + QLatin1String("='") + i.value() + QLatin1String("', ");
        } else {
            DEB(i.key() << "is empty key. skipping.");
        }
    }
    statement.chop(2); // Remove last comma
    statement.append(QLatin1String(" WHERE _rowid_=") + QString::number(position.second));

    //execute query
    DEB("UPDATE QUERY: " << statement);
    QSqlQuery q(statement);
    //check result. Upon success, error should be " "
    error = q.lastError().text();
    if (error.length() < 2) {
        DEB("Object successfully updated.");
        return true;
    } else {
        DEB("Query Error: " << q.lastError().text());
        return false;
    }
}

//Debug
void Entry::print()
{
    QString v = "Object status:\t\033[38;2;0;255;0;48;2;0;0;0m VALID \033[0m\n";
    QString nv = "Object status:\t\033[38;2;255;0;0;48;2;0;0;0m INVALID \033[0m\n";
    QTextStream cout(stdout, QIODevice::WriteOnly);

    cout << "=========Database Entry=========\n";
    //if(isValid){cout << v;}else{cout << nv;}
    cout << "Record from table: " << position.first << ", row: " << position.second << "\n";
    cout << "=================================\n";
    QMap<QString, QString>::const_iterator i;
    for (i = data.constBegin(); i != data.constEnd(); ++i) {
        cout << i.key() << ":\t" << i.value() << "\n";
    }
}

QString Entry::debug()
{
    print();
    return QString();
}
