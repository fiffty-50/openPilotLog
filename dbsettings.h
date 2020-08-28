#ifndef DBSETTINGS_H
#define DBSETTINGS_H

#include <QCoreApplication>

/*!
 * \brief The dbSettings class provides functionality for retreiving settings
 * from the database. In general, most values are provided as either QString or
 * QVector<QString>.
 */
class dbSettings
{
public:

    static void storeSetting(int setting_id, QString setting_value);

    static QString retreiveSetting(int setting_id);

    static QVector<QString> retreiveSettingInfo(QString setting_id);
};

#endif // DBSETTINGS_H
