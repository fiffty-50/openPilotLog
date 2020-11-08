#ifndef TOTALSWIDGET_H
#define TOTALSWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include "src/database/db.h"
#include "src/classes/stat.h"

namespace Ui {
class TotalsWidget;
}

class TotalsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TotalsWidget(QWidget *parent = nullptr);
    ~TotalsWidget();

private:
    Ui::TotalsWidget *ui;
};

#endif // TOTALSWIDGET_H
