#ifndef STYLEDAIRPORTDELEGATE_H
#define STYLEDAIRPORTDELEGATE_H

#include <QStyledItemDelegate>

class StyledAirportDelegate : public QStyledItemDelegate {
  public:
    enum DisplayStyle {Icao, Iata, Name };
    explicit StyledAirportDelegate(DisplayStyle style = Icao, QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
  private:
    const DisplayStyle m_style;

};

#endif // STYLEDAIRPORTDELEGATE_H
