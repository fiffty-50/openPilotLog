#ifndef PILOTTABLEEDITWIDGET_H
#define PILOTTABLEEDITWIDGET_H

#include "tableeditwidget.h"
#include "src/database/pilotentry.h"

class PilotTableEditWidget : public TableEditWidget
{
public:
    PilotTableEditWidget(QWidget *parent = nullptr);

    virtual void setupModelAndView() override;
    virtual void setupUI() override;
    virtual EntryEditDialog *getEntryEditDialog(QWidget *parent = nullptr) override;


private:
    const int COL_ROWID = 0;
    const int COL_LASTNAME = 1;
    const int COL_FIRSTNAME = 2;
    const int COL_COMPANY = 4;
    const int COLS_TO_HIDE[5] = {0, 3, 5, 6, 7};

    const static inline QStringList FILTER_COLUMNS = { tr("First Name"), tr("Last Name"), tr("Company") };
    const static inline QStringList FILTER_COLUMN_NAMES = {
                                                            OPL::PilotEntry::FIRSTNAME,
                                                            OPL::PilotEntry::LASTNAME,
                                                            OPL::PilotEntry::COMPANY };

    /*!
    * \brief Informs the user that deleting a Pilot has been unsuccessful
    *
    * \details Normally, when one of these entries can not be deleted, it is because of
    * a [foreign key constraint](https://sqlite.org/foreignkeys.html), meaning that a flight
    * is associated with the Pilot that was supposed to be deleted as Pilot-in-command.
    *
    * This function is used to inform the user and give hints on how to solve the problem.
    */
    virtual QString deleteErrorString(int pilotId) override;

    virtual QString confirmDeleteString(int rowId) override;

private slots:
    virtual void filterTextChanged(const QString &filterText) override;
};

#endif // PILOTTABLEEDITWIDGET_H
