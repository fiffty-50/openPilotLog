#ifndef NEWSIMDIALOG_H
#define NEWSIMDIALOG_H

#include <QDialog>
#include "src/classes/asimulatorentry.h"
#include "src/database/adatabase.h"
#include "src/classes/acompletiondata.h"

namespace Ui {
class NewSimDialog;
}

/*!
 * \brief The NewSimDialog class enables adding a new Simulator Session to the database or editing an existing one.
 * \details The NewSimDialog offers two constructors, one is used to create a new Simulator Entry
 * from scratch, while the other one is used to edit an existing entry. The existing entry
 * is identified by its ROW ID in the database and is then retreived, its data being used
 * to pre-fill the UI to enable editing the existing data.
 *
 * A QCompleter provides in-line completion for the aircraft type field.
 */
class NewSimDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewSimDialog(QWidget *parent = nullptr);
    explicit NewSimDialog(RowId_T row_id, QWidget *parent = nullptr);
    ~NewSimDialog();

private slots:
    void on_buttonBox_accepted();

    void on_dateLineEdit_editingFinished();

    void on_totalTimeLineEdit_editingFinished();

    void on_helpPushButton_clicked();

    void on_registrationLineEdit_textChanged(const QString &arg1);

private:
    Ui::NewSimDialog *ui;
    void init();
    void fillEntryData();
    bool verifyInput(QString &error_msg);
    RowData_T collectInput();

    ASimulatorEntry entry;
};

#endif // NEWSIMDIALOG_H
