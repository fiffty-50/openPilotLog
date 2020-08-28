#include "dbsettings.h"
#include "dbman.cpp"

/*
 * Settings Database Related Functions
 */
/*!
 * \brief storesetting Updates a stored setting in the database
 * \param setting_id
 * \param setting_value
 */
void dbSettings::storeSetting(int setting_id, QString setting_value)
{
    QSqlQuery query;
    query.prepare("UPDATE settings "
                  "SET  setting = ? "
                  "WHERE setting_id = ?");
    query.addBindValue(setting_value);
    query.addBindValue(setting_id);
    query.exec();
}

/*!
 * \brief retreiveSetting Looks up a setting in the database and returns its value
 * \param setting_id
 * \return setting value
 */
QString dbSettings::retreiveSetting(int setting_id)
{
    QSqlQuery query;
    query.prepare("SELECT setting FROM settings WHERE setting_id = ?");
    query.addBindValue(setting_id);
    query.exec();

    QString setting = "-1";

    while(query.next()){
        setting = query.value(0).toString();
    }
    return setting;
}

/*!
 * \brief retreiveSettingInfo Looks up a setting in the database and returns its value and description
 * \param setting_id
 * \return {setting_id, setting, description}
 */
QVector<QString> dbSettings::retreiveSettingInfo(QString setting_id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM settings WHERE setting_id = ?");
    query.addBindValue(setting_id);
    query.exec();

    QVector<QString> setting;

    while(query.next()){
        setting.append(query.value(0).toString());
        setting.append(query.value(1).toString());
        setting.append(query.value(2).toString());
    }
    return setting;
}
