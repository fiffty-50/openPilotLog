#ifndef NEWSIMDIALOG_H
#define NEWSIMDIALOG_H

#include <QDialog>
#include "src/classes/asimulatorentry.h"
#include "src/database/adatabase.h"
#include "src/classes/acompletiondata.h"

namespace Ui {
class NewSimDialog;
}

class NewSimDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewSimDialog(QWidget *parent = nullptr);
    ~NewSimDialog();

private slots:
    void on_buttonBox_accepted();

    void on_dateLineEdit_editingFinished();

    void on_timeLineEdit_editingFinished();

    void on_helpPushButton_clicked();

    void on_registrationLineEdit_textChanged(const QString &arg1);

private:
    Ui::NewSimDialog *ui;

    bool verifyInput(QString &error_msg);
    RowData_T collectInput();
};

#endif // NEWSIMDIALOG_H
