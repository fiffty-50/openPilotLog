#ifndef PILOTTABLEEDITWIDGET_H
#define PILOTTABLEEDITWIDGET_H

#include "tableeditwidget.h"
#include "src/database/pilotentry.h"

class PilotTableEditWidget : public TableEditWidget
{
    Q_OBJECT
public:
    PilotTableEditWidget(QWidget *parent = nullptr);

    virtual void setupModelAndView() override;
    virtual void setupUI() override;
    virtual EntryEditDialog *getEntryEditDialog(QWidget *parent = nullptr) override;


private:
    static constexpr int COL_ROWID = 0;
    static constexpr int COL_LASTNAME = 1;
    static constexpr int COL_FIRSTNAME = 2;
    static constexpr int COL_COMPANY = 4;
    static constexpr int COLS_TO_HIDE[5] = {0, 3, 5, 6, 7};

    const QString COLUMN_1_NAME = tr("First Name");
    const QString COLUMN_2_NAME = tr("Last Name");
    const QString COLUMN_3_NAME = tr("Company");

    const QStringList FILTER_COLUMNS = { COLUMN_1_NAME, COLUMN_2_NAME, COLUMN_3_NAME };
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
