#ifndef TOTALSWIDGET_H
#define TOTALSWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <QLabel>
#include <QLineEdit>
#include "src/database/db.h"
#include "src/classes/stat.h"

namespace Ui {
class totalsWidget;
}

class totalsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit totalsWidget(QWidget *parent = nullptr);
    ~totalsWidget();

private:
    Ui::totalsWidget *ui;
};

#endif // TOTALSWIDGET_H
