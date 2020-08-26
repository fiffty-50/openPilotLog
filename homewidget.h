#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>

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


    void on_debugButton_clicked();

private:
    Ui::homeWidget *ui;
};

#endif // HOMEWIDGET_H
