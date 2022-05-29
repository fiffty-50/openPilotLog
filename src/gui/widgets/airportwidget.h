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

private slots:
    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_searchComboBox_currentIndexChanged(int index);

private:
    Ui::AirportWidget *ui;
    QSqlTableModel *model;
    QTableView *view;

    void setupModelAndeView();
    void setupSearch();
};

#endif // AIRPORTWIDGET_H
