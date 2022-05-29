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
    ~NewAirportDialog();

private slots:
    void on_buttonBox_accepted();

    void on_iataLineEdit_textChanged(const QString &arg1);

    void on_icaoLineEdit_textChanged(const QString &arg1);

    void on_nameLineEdit_editingFinished();

    void on_iataLineEdit_editingFinished();

    void on_icaoLineEdit_editingFinished();

    void on_latitudeLineEdit_editingFinished();

    void on_longitudeLineEdit_editingFinished();

    void on_latitudeLineEdit_inputRejected();

    void on_longitudeLineEdit_inputRejected();

    void on_icaoLineEdit_inputRejected();

    void on_iataLineEdit_inputRejected();

private:
    Ui::NewAirportDialog *ui;
    void setValidators();
};

#endif // NEWAIRPORTDIALOG_H
