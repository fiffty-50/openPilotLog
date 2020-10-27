#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include "src/database/db.h"
#include "src/classes/stat.h"
#include "src/classes/calc.h"
#include "src/classes/completionlist.h"
#include "src/gui/dialogues/newtail.h"
#include "src/classes/aircraft.h"

namespace Ui {
class homeWidget;
}

class homeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit homeWidget(QWidget *parent = nullptr);
    ~homeWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::homeWidget *ui;
};

#endif // HOMEWIDGET_H
