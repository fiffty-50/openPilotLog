#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class settingsWidget;
}

class settingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit settingsWidget(QWidget *parent = nullptr);
    ~settingsWidget();

private:
    Ui::settingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
