#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include <QDialog>

namespace Ui {
class FirstRunDialog;
}

class FirstRunDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FirstRunDialog(QWidget *parent = nullptr);
    ~FirstRunDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::FirstRunDialog *ui;
    void disregard();
};

#endif // FIRSTRUNDIALOG_H
