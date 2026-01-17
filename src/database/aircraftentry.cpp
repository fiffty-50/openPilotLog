/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "aircraftentry.h"
#include <QSqlQuery>

namespace OPL {

AircraftEntry::AircraftEntry()
    : Row(DbTable::v2AircraftTypes, &FIELDS)
{}

AircraftEntry::AircraftEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::v2AircraftTypes, row_id, row_data, &FIELDS)
{}

bool AircraftEntry::isValid() const
{
    if(!m_rowData.contains(MAKE) || m_rowData.value(MAKE).toString().isEmpty()) {
        LOG << QStringLiteral("Invalid aircraft entry: Make is missing");
        return false;
    }
    if(!m_rowData.contains(MODEL) || m_rowData.value(MODEL).toString().isEmpty()) {
        LOG << QStringLiteral("Invalid aircraft entry: Model is missing");
        return false;
    }
    if(!m_rowData.contains(ENGINE_TYPE) || !ENGINE_TYPES.contains(m_rowData.value(ENGINE_TYPE).toString())) {
        LOG << QStringLiteral("Invalid aircraft entry: Engine type is missing or invalid");
        return false;
    }
    if(!m_rowData.contains(CLASS) || !AIRCRAFT_CLASSES.contains(m_rowData.value(CLASS).toString())) {
        LOG << QStringLiteral("Invalid aircraft entry: Aircraft class is missing or invalid");
        return false;
    }
    if(!m_rowData.contains(SUB_CLASS) || !AIRCRAFT_SUBCLASSES.contains(m_rowData.value(SUB_CLASS).toString())) {
        LOG << QStringLiteral("Invalid aircraft entry: Aircraft sub-class is missing or invalid");
        return false;
    }
    if(!m_rowData.contains(WAKE_CATEGORY) || !WAKE_CATEGORIES.contains(m_rowData.value(WAKE_CATEGORY).toString())) {
        LOG << QStringLiteral("Invalid aircraft entry: Wake category is missing or invalid");
        return false;
    }
    if(!m_rowData.contains(IS_MULTI_ENGINE)) {
        LOG << QStringLiteral("Invalid aircraft entry: Is multi-engine flag is missing");
        return false;
    }
    if(!m_rowData.contains(IS_MULTI_PILOT)) {
        LOG << QStringLiteral("Invalid aircraft entry: Is multi-pilot flag is missing");
        return false;
    }
    return true;
}

QString AircraftEntry::getTypeString(int aircraft_id)
{
    const QString statement = QStringLiteral("SELECT make||' '||model AS ident "
                                        "FROM aircraft_types "
                                        "WHERE model IS NOT NULL "
                                        "AND variant IS NULL "
                                        "AND aircraft_type_id = ? "
                                        "UNION "
                                        "SELECT make||' '||model||'-'||variant AS ident "
                                        "FROM aircraft_types "
                                        "WHERE variant IS NOT NULL "
                                        "AND aircraft_type_id = ?");
    QSqlQuery query;
    query.prepare(statement);
    query.setForwardOnly(true);
    query.exec();

    if(!query.next()) {
        return QStringLiteral("Aircraft type not in database");
    } else {
        return query.value(0).toString();
    }
}

// Setters
bool AircraftEntry::setMake(const QString &input)
{
    if(input.isEmpty()) {
        return false;
    }

    m_rowData.insert(MAKE, input);
    return true;
}

bool AircraftEntry::setModel(const QString &input)
{
    if(input.isEmpty()) {
        return false;
    }

    m_rowData.insert(MODEL, input);
    return true;
}

bool AircraftEntry::setVariant(const QString &input)
{
    m_rowData.insert(VARIANT, input);
    return true;
}

bool AircraftEntry::setIcaoDesignator(const QString &input)
{
    m_rowData.insert(ICAO_DESIGNATOR, input);
    return true;
}

bool AircraftEntry::setEngineType(EngineType type)
{
    m_rowData.insert(ENGINE_TYPE, toString(type));
    return true;
}

bool AircraftEntry::setIsMultiEngine(bool input)
{
    m_rowData.insert(IS_MULTI_ENGINE, input);
    return true;
}

bool AircraftEntry::setClass(AircraftClass cls)
{
    m_rowData.insert(CLASS, toString(cls));
    return true;
}

bool AircraftEntry::setSubClass(AircraftSubClass cls)
{
    m_rowData.insert(SUB_CLASS, toString(cls));
    return true;
}

bool AircraftEntry::setIsMultiPilot(bool input)
{
    m_rowData.insert(IS_MULTI_PILOT, input);
    return true;
}

bool AircraftEntry::setWakeCategory(WakeCategory cat)
{
    m_rowData.insert(WAKE_CATEGORY, toString(cat));
    return true;
}

bool AircraftEntry::setTypeRating(const QString &input)
{
    m_rowData.insert(TYPE_RATING, input);
    return true;
}

bool AircraftEntry::setRemarks(const QString &input)
{
    m_rowData.insert(REMARKS, input);
    return true;
}
// Getters
QString AircraftEntry::getMake() const
{
    return m_rowData.value(MAKE).toString();
}
QString AircraftEntry::getModel() const
{
    return m_rowData.value(MODEL).toString();
}
QString AircraftEntry::getVariant() const
{
    return m_rowData.value(VARIANT).toString();
}
QString AircraftEntry::getIcaoDesignator() const
{
    return m_rowData.value(ICAO_DESIGNATOR).toString();
}
AircraftEntry::EngineType AircraftEntry::getEngineType() const
{
    QString value = m_rowData.value(ENGINE_TYPE).toString();
    return static_cast<EngineType>(ENGINE_TYPES.indexOf(value));
}
bool AircraftEntry::getIsMultiEngine() const
{
    return m_rowData.value(IS_MULTI_ENGINE).toBool();
}
AircraftEntry::AircraftClass AircraftEntry::getClass() const
{
    QString value = m_rowData.value(CLASS).toString();
    return static_cast<AircraftClass>(AIRCRAFT_CLASSES.indexOf(value));
}
AircraftEntry::AircraftSubClass AircraftEntry::getSubClass() const
{
    QString value = m_rowData.value(SUB_CLASS).toString();
    return static_cast<AircraftSubClass>(AIRCRAFT_SUBCLASSES.indexOf(value));
}
bool AircraftEntry::getIsMultiPilot() const
{
    return m_rowData.value(IS_MULTI_PILOT).toBool();
}
AircraftEntry::WakeCategory AircraftEntry::getWakeCategory() const
{
    QString value = m_rowData.value(WAKE_CATEGORY).toString();
    return static_cast<WakeCategory>(WAKE_CATEGORIES.indexOf(value));
}
QString AircraftEntry::getTypeRating() const
{
    return m_rowData.value(TYPE_RATING).toString();
}
QString AircraftEntry::getRemarks() const
{
    return m_rowData.value(REMARKS).toString();
}

// Combo Boxes for use in the GUI - these map the database values to translatable strings for display
void AircraftEntry::setupEngineTypeComboBox(QComboBox *comboBox)
{
    if(!comboBox) {
        return;
    }
    comboBox->clear();
    comboBox->addItem(QObject::tr("Unpowered"), static_cast<int>(EngineType::UNPOWERED));
    comboBox->addItem(QObject::tr("Piston"), static_cast<int>(EngineType::PISTON));
    comboBox->addItem(QObject::tr("Turboprop"), static_cast<int>(EngineType::TURBOPROP));
    comboBox->addItem(QObject::tr("Jet"), static_cast<int>(EngineType::JET));
    comboBox->addItem(QObject::tr("Electric"), static_cast<int>(EngineType::ELECTRIC));
    comboBox->addItem(QObject::tr("Other"), static_cast<int>(EngineType::OTHER));
}

void AircraftEntry::setupAircraftClassComboBox(QComboBox *comboBox)
{
    if(!comboBox) {
        return;
    }
    comboBox->clear();
    comboBox->addItem(QObject::tr("Aeroplane"), static_cast<int>(AircraftClass::AEROPLANE));
    comboBox->addItem(QObject::tr("Rotorcraft"), static_cast<int>(AircraftClass::ROTORCRAFT));
    comboBox->addItem(QObject::tr("Sailplane"), static_cast<int>(AircraftClass::SAILPLANE));
    comboBox->addItem(QObject::tr("Balloon"), static_cast<int>(AircraftClass::BALLOON));
    comboBox->addItem(QObject::tr("Airship"), static_cast<int>(AircraftClass::AIRSHIP));
    comboBox->addItem(QObject::tr("Other"), static_cast<int>(AircraftClass::OTHER));
}

void AircraftEntry::setupAircraftSubClassComboBox(QComboBox *comboBox)
{
    if(!comboBox) {
        return;
    }
    comboBox->clear();
    comboBox->addItem(QObject::tr("Land"), static_cast<int>(AircraftSubClass::LAND));
    comboBox->addItem(QObject::tr("Sea"), static_cast<int>(AircraftSubClass::SEA)); 
}

void AircraftEntry::setupWakeCategoryComboBox(QComboBox *comboBox)
{
    if(!comboBox) {
        return;
    }
    comboBox->clear();
    comboBox->addItem(QObject::tr("Light"), static_cast<int>(WakeCategory::LIGHT));
    comboBox->addItem(QObject::tr("Medium"), static_cast<int>(WakeCategory::MEDIUM));
    comboBox->addItem(QObject::tr("Heavy"), static_cast<int>(WakeCategory::HEAVY));
    comboBox->addItem(QObject::tr("Super"), static_cast<int>(WakeCategory::SUPER));
}

void AircraftEntry::setupIsMultiEngineComboBox(QComboBox *comboBox)
{
    comboBox->addItem(QObject::tr("Single Engine"), 0);
    comboBox->addItem(QObject::tr("Multi Engine"), 1);
}

void AircraftEntry::setupIsMultiPilotComboBox(QComboBox *comboBox)
{
    comboBox->addItem(QObject::tr("Single Pilot"), 0);
    comboBox->addItem(QObject::tr("Multi Pilot"), 1);
}   




} // namespace OPL
