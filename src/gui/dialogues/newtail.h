#ifndef NEWTAIL_H
#define NEWTAIL_H

#include <QDialog>
#include <QCompleter>
#include "src/classes/completionlist.h"

namespace Ui {
class NewTail;
}

class NewTail : public QDialog
{
    Q_OBJECT

public:
    explicit NewTail(QWidget *parent = nullptr);
    ~NewTail();

private slots:

    void on_searchLineEdit_textChanged(const QString &arg1);

private:
    Ui::NewTail *ui;

    QStringList aircraftlist;
};

#endif // NEWTAIL_H
