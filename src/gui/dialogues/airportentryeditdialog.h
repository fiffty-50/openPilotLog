#ifndef NEWAIRPORTDIALOG_H
#define NEWAIRPORTDIALOG_H

#include "src/gui/dialogues/entryeditdialog.h"

namespace Ui {
class NewAirportDialog;
}

class NewAirportDialog : public EntryEditDialog
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
    int m_rowId;

    void setValidators();
    void loadTimeZones();
    bool confirmTimezone();
    void loadAirportData(int row_id);
    bool verifyInput();


    // EntryEditDialog interface
public:
    virtual void loadEntry(int rowId) override;
    virtual bool deleteEntry(int rowId) override;
};

#endif // NEWAIRPORTDIALOG_H
