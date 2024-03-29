#ifndef AIRPORTWIDGET_H
#define AIRPORTWIDGET_H

#include <QWidget>
#include <QSqlTableModel>
#include <QTableView>

namespace Ui {
class AirportWidget;
}

class AirportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AirportWidget(QWidget *parent = nullptr);
    ~AirportWidget();

signals:
    void airportDatabaseUpdated();

private slots:
    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_searchComboBox_currentIndexChanged(int index);

    void on_newAirportPushButton_clicked();

    void onSelectionChanged();

    void on_deletePushButton_clicked();

    void on_editAirportPushButton_clicked();

    void refresh();

private:
    Ui::AirportWidget *ui;
    QSqlTableModel *model;
    QTableView *view;
    QList<int> selectedEntries;

    void setupModelAndeView();
    void setupSearch();

    inline const static QString TABLE_NAME = QStringLiteral("airports");
    inline const static QMap<int, QString> FILTER_MAP = {
        {0, QStringLiteral("icao LIKE \"%")},
        {1, QStringLiteral("iata LIKE \"%")},
        {2, QStringLiteral("name LIKE \"%")},
        {3, QStringLiteral("country LIKE \"%")},
    };
    inline const static QMap<int, QString> HEADER_MAP = {
        {0, QStringLiteral("ICAO")},
        {1, QStringLiteral("IATA")},
        {2, QStringLiteral("Name")},
        {3, QStringLiteral("Country")},
    };
};

#endif // AIRPORTWIDGET_H
