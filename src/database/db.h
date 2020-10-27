#ifndef DB_H
#define DB_H

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QDebug>

class sql
{
public:
    enum tableName {flights, pilots, settings };
    enum queryType {select, update };
    enum matchType {exactMatch, partialMatch};
};


class db
{
public:

    static void connect();

    static QString sqliteversion();

    static QVector<QString> getColumnNames(QString table);


    static bool exists(QString column, QString table, QString checkColumn, QString value, sql::matchType match);

    static QString singleSelect(QString column, QString table, QString checkColumn, QString value, sql::matchType match);

    static QVector<QString> multiSelect(QVector<QString> columns, QString table, QString checkColumn, QString value, sql::matchType match);

    static QVector<QString> multiSelect(QVector<QString> columns, QString table);

    static bool singleUpdate(QString table, QString column, QString value, QString checkColumn, QString checkvalue, sql::matchType match);

    static bool deleteRow(QString table, QString column, QString value, sql::matchType match);

    static QVector<QString> customQuery(QString query, int returnValues);

};

#endif // DB_H
