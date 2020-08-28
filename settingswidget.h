#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include "dbsettings.h"

namespace Ui {
class settingsWidget;
}

class settingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit settingsWidget(QWidget *parent = nullptr);
    ~settingsWidget();

private slots:
    void on_flightNumberPrefixLineEdit_textEdited(const QString &arg1);

private:
    Ui::settingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
