#ifndef NEWAIRPORTDIALOG_H
#define NEWAIRPORTDIALOG_H

#include <QDialog>

namespace Ui {
class NewAirportDialog;
}

class NewAirportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewAirportDialog(QWidget *parent = nullptr);
    explicit NewAirportDialog(int row_id, QWidget* parent = nullptr);
    ~NewAirportDialog();


private slots:
    void on_buttonBox_accepted();

    void on_iataLineEdit_textChanged(const QString &arg1);

    void on_icaoLineEdit_textChanged(const QString &arg1);

    void on_buttonBox_rejected();

private:
    Ui::NewAirportDialog *ui;
    void setValidators();
    void loadTimeZones();
    bool confirmTimezone();
    void loadAirportData(int row_id);
    bool verifyInput();
    int rowId;
};

#endif // NEWAIRPORTDIALOG_H
