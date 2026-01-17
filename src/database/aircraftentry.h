/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#ifndef AIRCRAFTENTRY_H
#define AIRCRAFTENTRY_H
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing an Aircraft entry.
 * \details
 * In this context an aircraft refers to the aircraft TYPE (Boeing 737, Airbus A320,...) and
 * could be seen as an analogy for a class. The aircraft 'instances' are called TAILS and are
 * stored in the tails database (A Boeing 737 with registration (tail) LN-ENL for example).
 *
 * Aircraft - All aircraft with a common type that share its traits
 * Tail - A specific aircraft of that type
 *
 * The aircraft table in the database contains templates of various aircraft types
 * and is used to provide auto-completion when the user adds a new tail to the logbook.
 */
class AircraftEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("aircraft_types");
public:
    AircraftEntry();
    AircraftEntry(const RowData_T &row_data);
    AircraftEntry(int row_id, const RowData_T &row_data);

    bool isValid() const override;

    /*!
     * \brief returns "Make Model" or if available "Make Model-Variant" for a given aircraft_id
     */
    static QString getTypeString(int aircraft_id);

    enum class EngineType {
        UNPOWERED,
        PISTON,
        TURBOPROP,
        JET,
        ELECTRIC,
        OTHER
    };
    bool setEngineType(EngineType type);
    EngineType getEngineType() const;
    
    enum class AircraftClass {
        AEROPLANE,
        ROTORCRAFT,
        SAILPLANE,
        BALLOON,
        AIRSHIP,
        OTHER
    };
    bool setClass(AircraftClass cls);
    AircraftClass getClass() const;

    enum class AircraftSubClass {
        LAND,
        SEA
    };
    bool setSubClass(AircraftSubClass subCls);
    AircraftSubClass getSubClass() const;

    enum class WakeCategory {
        LIGHT,
        MEDIUM,
        HEAVY,
        SUPER
    };
    bool setWakeCategory(WakeCategory category);
    WakeCategory getWakeCategory() const;

    bool setMake(const QString &input);
    bool setModel(const QString &input);
    bool setVariant(const QString &input);
    bool setIcaoDesignator(const QString &input);
    bool setIsMultiEngine(bool input);
    bool setIsMultiPilot(bool input);
    bool setTypeRating(const QString &input);
    bool setRemarks(const QString &input);

    QString getMake() const;
    QString getModel() const;
    QString getVariant() const;
    QString getIcaoDesignator() const;
    bool getIsMultiEngine() const;
    bool getIsMultiPilot() const;
    QString getTypeRating() const;
    QString getRemarks() const;

    static void setupEngineTypeComboBox(QComboBox *comboBox);
    static void setupAircraftClassComboBox(QComboBox *comboBox);
    static void setupAircraftSubClassComboBox(QComboBox *comboBox);
    static void setupWakeCategoryComboBox(QComboBox *comboBox);
    static void setupIsMultiEngineComboBox(QComboBox *comboBox);
    static void setupIsMultiPilotComboBox(QComboBox *comboBox);
    
private:
    static const inline QString MAKE = QStringLiteral("make");
    static const inline QString MODEL = QStringLiteral("model");
    static const inline QString VARIANT = QStringLiteral("variant");
    static const inline QString ICAO_DESIGNATOR = QStringLiteral("icao_designator");
    static const inline QString ENGINE_TYPE = QStringLiteral("engine_type");
    static const inline QString IS_MULTI_ENGINE = QStringLiteral("is_multi_engine");
    static const inline QString CLASS = QStringLiteral("class");
    static const inline QString SUB_CLASS = QStringLiteral("sub_class");
    static const inline QString IS_MULTI_PILOT = QStringLiteral("is_multi_pilot");
    static const inline QString WAKE_CATEGORY = QStringLiteral("wake_category");
    static const inline QString TYPE_RATING = QStringLiteral("type_rating");
    static const inline QString REMARKS = QStringLiteral("remarks");

private:
    const static inline QList<QString> FIELDS = {
        MAKE,
        MODEL,
        VARIANT,
        ICAO_DESIGNATOR,
        ENGINE_TYPE,
        IS_MULTI_ENGINE,
        CLASS,
        SUB_CLASS,
        IS_MULTI_PILOT,
        WAKE_CATEGORY,
        TYPE_RATING,
        REMARKS
    };
    

    const static inline QList<QString> ENGINE_TYPES = {
        QStringLiteral("UNPOWERED"),
        QStringLiteral("PISTON"),
        QStringLiteral("TURBOPROP"),
        QStringLiteral("JET"),
        QStringLiteral("ELECTRIC"),
        QStringLiteral("OTHER"),
    };
    
    static QString toString(EngineType type) {
        switch (type) {
            case EngineType::UNPOWERED: return ENGINE_TYPES[0];
            case EngineType::PISTON:    return ENGINE_TYPES[1];
            case EngineType::TURBOPROP: return ENGINE_TYPES[2];
            case EngineType::JET:       return ENGINE_TYPES[3];
            case EngineType::ELECTRIC:  return ENGINE_TYPES[4];
            case EngineType::OTHER:     return ENGINE_TYPES[5];
            default:                    return {};
        }
    }
    

    
    const static inline QList<QString> AIRCRAFT_CLASSES = {
        QStringLiteral("AEROPLANE"),
        QStringLiteral("ROTORCRAFT"),
        QStringLiteral("SAILPLANE"),
        QStringLiteral("BALLOON"),
        QStringLiteral("AIRSHIP"),
        QStringLiteral("OTHER"),
    };

    static QString toString(AircraftClass cls) {
        switch (cls) {
            case AircraftClass::AEROPLANE:  return AIRCRAFT_CLASSES[0];
            case AircraftClass::ROTORCRAFT: return AIRCRAFT_CLASSES[1];
            case AircraftClass::SAILPLANE:  return AIRCRAFT_CLASSES[2];
            case AircraftClass::BALLOON:    return AIRCRAFT_CLASSES[3];
            case AircraftClass::AIRSHIP:    return AIRCRAFT_CLASSES[4];
            case AircraftClass::OTHER:      return AIRCRAFT_CLASSES[5];
            default:                        return {};
        }
    }

    const static inline QList<QString> AIRCRAFT_SUBCLASSES = {
        QStringLiteral("LAND"),
        QStringLiteral("SEA"),
    };
    
    static QString toString(AircraftSubClass subCls) {
        switch (subCls) {
            case AircraftSubClass::LAND: return AIRCRAFT_SUBCLASSES[0];
            case AircraftSubClass::SEA:  return AIRCRAFT_SUBCLASSES[1];
            default:                     return {};
        }
    }

    const static inline QList<QString> WAKE_CATEGORIES = {
        QStringLiteral("LIGHT"),
        QStringLiteral("MEDIUM"),
        QStringLiteral("HEAVY"),
        QStringLiteral("SUPER"),
    };

    static QString toString(WakeCategory category) {
        switch (category) {
            case WakeCategory::LIGHT:  return WAKE_CATEGORIES[0];
            case WakeCategory::MEDIUM: return WAKE_CATEGORIES[1];
            case WakeCategory::HEAVY:  return WAKE_CATEGORIES[2];
            case WakeCategory::SUPER:  return WAKE_CATEGORIES[3];
            default:                   return {};
        }
    }
};

} // namespace OPL


#endif // AIRCRAFTENTRY_H
