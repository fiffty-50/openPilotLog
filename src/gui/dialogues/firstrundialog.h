#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QMessageBox>

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

    void on_previousPushButton_clicked();

    void on_nextPushButton_clicked();

    void on_themeGroup_toggled(int id);



private:
    Ui::FirstRunDialog *ui;
    // [G]: finish is the old signal.
    // finishSetup does something with template of database which
    // goes over my head but everything works for now. Better naming needed
    bool finishSetup();
    void finish();

};

#endif // FIRSTRUNDIALOG_H
