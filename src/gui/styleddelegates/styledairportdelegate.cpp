#include "styledairportdelegate.h"
#include "src/database/cache/airportinfo.h"

StyledAirportDelegate::StyledAirportDelegate(DisplayStyle style, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_style(style) {}

QString StyledAirportDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    switch (m_style) {
    case Icao:
        return airportData->icao(value.toInt());
    case Iata:
        return airportData->iata(value.toInt());
    case Name:
        return airportData->nameFromRowId(value.toInt());
    default:
        return airportData->icao(value.toInt());
    }
}
