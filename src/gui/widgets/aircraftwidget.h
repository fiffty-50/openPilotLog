#ifndef AIRCRAFTWIDGET_H
#define AIRCRAFTWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QSqlTableModel>
#include <QDebug>
#include <QLabel>
#include "src/gui/dialogues/newtail.h"
#include "src/classes/aircraft.h"
#include "src/database/db.h"

namespace Ui {
class aircraftWidget;
}

class aircraftWidget : public QWidget
{
    Q_OBJECT

public:
    explicit aircraftWidget(QWidget *parent = nullptr);
    ~aircraftWidget();

    qint32 selectedAircraft = 0;

    void setSelectedAircraft(const qint32 &value);

private slots:
    void tableView_selectionChanged(const QItemSelection &index, const QItemSelection &);

    void on_deleteButton_clicked();

private:
    Ui::aircraftWidget *ui;
};

#endif // AIRCRAFTWIDGET_H
