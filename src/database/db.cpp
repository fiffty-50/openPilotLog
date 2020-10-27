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
#include "db.h"

// Debug Makro
#define DEB(expr) \
    qDebug() << "db ::" << __func__ << "\t" << expr

/*!
 * \brief db::connect connects to the database via the default connection.
 * Can then be accessed globally with QSqlDatabase::database("qt_sql_default_connection")
 */
void db::connect()
{
    const QString driver("QSQLITE");

    if(QSqlDatabase::isDriverAvailable(driver)){

        QDir directory("data");
        QString databaseLocation = directory.filePath("logbook.db");
        QSqlDatabase db = QSqlDatabase::addDatabase(driver);
        db.setDatabaseName(databaseLocation);

        if(!db.open()){
            DEB("DatabaseConnect - ERROR: " << db.lastError().text());
        }else{
            DEB("Database connection established.");
        }
    }else{
        DEB("DatabaseConnect - ERROR: no driver " << driver << " available");
    }
}

/*!
 * \brief db::sqliteversion queries database version.
 */
QString db::sqliteversion()
{
    QSqlQuery version;
    version.prepare("SELECT sqlite_version()");
    version.exec();
    QString result;

    while (version.next()) {
        result.append(version.value(0).toString());
    }
    return result;
}


/*!
 * \brief db::exists checks if a certain value exists in the database with a sqlite WHERE statement
 * \param table - Name of the table
 * \param column - Name of the column
 * \param value - The value to be checked
 * \return
 */
bool db::exists(QString column, QString table, QString checkColumn, QString value, sql::matchType match)
{
    bool output = false;
    QString statement = "SELECT " + column + " FROM " + table + " WHERE " + checkColumn;

    switch (match) {
    case sql::exactMatch:
        statement += " = '" + value + QLatin1Char('\'');
        break;
    case sql::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(" LIKE '" + value + QLatin1Char('\''));
        break;
    }

    DEB(statement);

    QSqlQuery q(statement);
    q.exec();

    if(!q.first()){
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
    }else{
        DEB("Success. Found a result.");
        output = true;
        if(q.next()){
            DEB("More than one result in Database for your query");
        }
    }
// Debug:
    q.first();
    q.previous();
    while(q.next()){
            DEB("Query result: " << q.value(0).toString());
    }
// end of Debug
    return output;
}

/*!
 * \brief singleSelect Returns a single value from the database with a sqlite WHERE statement
 * \param table - Name of the table
 * \param column - Name of the column
 * \param value - Identifier for WHERE statement
 * \param match - enum sql::exactMatch or sql::partialMatch
 * \return QString
 */
QString db::singleSelect(QString column, QString table, QString checkColumn, QString value, sql::matchType match)
{
    QString statement = "SELECT " + column + " FROM " + table + " WHERE " + checkColumn;
    QString result;

    switch (match) {
    case sql::exactMatch:
        statement += " = '" + value + QLatin1Char('\'');
        break;
    case sql::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(" LIKE '" + value + QLatin1Char('\''));
        break;
    }

    DEB(statement);

    QSqlQuery q(statement);
    q.exec();

    if(!q.first()){
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
        return QString();
    }else{
        DEB("Success. Found a result.");
        result.append(q.value(0).toString());
        if(q.next()){
            DEB("More than one result in Database for your query");
        }
        return result;
    }
}

/*!
 * \brief db::multiSelect Returns multiple values from the database with a sqlite WHERE statement
 * \param table - Name of the table
 * \param columns - QVector<QString> Names of the columns to be queried
 * \param value - Identifier for WHERE statement
 * \param checkColumn - column to match value to
 * \param match - enum sql::exactMatch or sql::partialMatch
 * \return QVector<QString>
 */
QVector<QString> db::multiSelect(QVector<QString> columns, QString table, QString checkColumn, QString value, sql::matchType match)
{
    QString statement = "SELECT ";
    for(const auto& column : columns)
    {
        statement.append(column);
        if(column != columns.last())
        {
            statement.append(QLatin1String(", "));
        }
    }
    statement.append(" FROM " + table + " WHERE " + checkColumn);

    switch (match) {
    case sql::exactMatch:
        statement += " = '" + value + QLatin1Char('\'');
        break;
    case sql::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(" LIKE '" + value + QLatin1Char('\''));
        break;
    }

    DEB(statement);

    QSqlQuery q(statement);
    q.exec();

    if(!q.first()){
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
        return QVector<QString>();
    }else{
        q.first();
        q.previous();
        QVector<QString> result;
        while (q.next()) {
            for(int i = 0; i < columns.size() ; i++)
            {
                result.append(q.value(i).toString());
            }
        }
        return result;
    }
}
/*!
 * \brief db::multiSelect Returns a complete column(s) for a given table. Useful for creating
 * lists for QCompleter
 * \param column
 * \param table
 * \return
 */
QVector<QString> db::multiSelect(QVector<QString> columns, QString table)
{
    QString statement = "SELECT ";
    for(const auto& column : columns)
    {
        statement.append(column);
        if(column != columns.last())
        {
            statement.append(QLatin1String(", "));
        }
    }
    statement.append(" FROM " + table);

    DEB(statement);

    QSqlQuery q(statement);
    q.exec();

    if(!q.first()){
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
        return QVector<QString>();
    }else{
        q.first();
        q.previous();
        QVector<QString> result;
        while (q.next()) {
            for(int i = 0; i < columns.size() ; i++)
            {
                result.append(q.value(i).toString());
            }
        }
        return result;
    }
}

/*!
 * \brief db::singleUpdate Updates a single value in the database.
 * Query format: UPDATE table SET column = value WHERE checkcolumn =/LIKE checkvalue
 * \param table Name of the table to be updated
 * \param column Name of the column to be updated
 * \param checkColumn Name of the column for WHERE statement
 * \param value The value to be set
 * \param checkvalue The value for the WHERE statement
 * \param match enum sql::exactMatch or sql::partialMatch
 * \return true on success, otherwise error messages in debug out
 */
bool db::singleUpdate(QString table, QString column, QString value, QString checkColumn, QString checkvalue, sql::matchType match)
{
    QString statement = "UPDATE " + table;
    statement.append(QLatin1String(" SET ") + column + QLatin1String(" = '") + value);
    statement.append(QLatin1String("' WHERE "));

    switch (match) {
    case sql::exactMatch:
        statement.append(checkColumn + " = '" + checkvalue + QLatin1Char('\''));
        break;
    case sql::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(checkColumn + " LIKE '" + checkvalue + QLatin1Char('\''));
        break;
    }

    DEB(statement);

    QSqlQuery q(statement);
    q.exec();
    QString error = q.lastError().text();

    if(error.length() > 1)
    {
        DEB("Errors have occured: " << error);
        return false;
    }else
    {
        DEB("Success!");
        return true;
    }
}

/*!
 * \brief db::deleteRow Deletes a single row from the database.
 * Query format: DELETE FROM table WHERE column =/LIKE value
 * \param table - Name of the table
 * \param column - Name of the column
 * \param value - Identifier for WHERE statement
 * \param match - enum sql::exactMatch or sql::partialMatch
 * \return true on success, otherwise error messages in debug out
 */
bool db::deleteRow(QString table, QString column, QString value, sql::matchType match)
{
    QString statement = "DELETE FROM " + table + " WHERE ";
    statement.append(column);


    switch (match) {
    case sql::exactMatch:
        statement += " = '" + value + QLatin1Char('\'');
        break;
    case sql::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(" LIKE '" + value + QLatin1Char('\''));
        break;
    }

    DEB(statement);

    QSqlQuery q(statement);
    q.exec();
    QString error = q.lastError().text();

    if(error.length() > 1)
    {
        DEB("Errors have occured: " << error);
        return false;
    }else
    {
        DEB("Success!");
        return true;
    }
}

/*!
 * \brief db::customQuery Can be used to send a complex query to the database.
 * \param query - the full sql query statement
 * \param returnValues - the number of expected return values
 * \return QVector<QString> of results
 */
QVector<QString> db::customQuery(QString query, int returnValues)
{
    QSqlQuery q(query);
    DEB(query);
    q.exec();

    if(!q.first()){
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
        return QVector<QString>();
    }else{
        q.first();
        q.previous();
        QVector<QString> result;
        while (q.next()) {
            for(int i = 0; i < returnValues ; i++)
            {
                result.append(q.value(i).toString());
            }
        }
        return result;
    }
}

/*!
 * \brief db::getColumnNames Looks up column names of a given table
 * \param table name of the table in the database
 */
QVector<QString> db::getColumnNames(QString table)
{
    QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
    QVector<QString> columnNames;
    QSqlRecord fields = db.record(table);

    for(int i = 0; i < fields.count(); i++){
        columnNames << fields.field(i).name();
    }
    return columnNames;
}
