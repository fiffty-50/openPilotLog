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
    static constexpr int COL_NAME = 1;
    static constexpr int COL_ALIAS = 2;
    static constexpr int COL_EMPLOYEE_ID = 3;
    static constexpr int COL_COMPANY = 4;
    static constexpr std::array<int,4> COLUMNS_TO_HIDE = {0, 5, 6, 7 };

    const QString COL_HEADER_NAME	 		= tr("Name");
    const QString COL_HEADER_ALIAS 			= tr("Alias");
    const QString COL_HEADER_COMPANY 		= tr("Company");
    const QString COL_HEADER_EMPLOYEE_ID 	= tr("Employee ID");

    const QMap<int, QString> DISPLAY_COLUMNS = {
        {COL_NAME, 			COL_HEADER_NAME },
        {COL_ALIAS, 		COL_HEADER_ALIAS },
        {COL_COMPANY, 		COL_HEADER_COMPANY },
        {COL_EMPLOYEE_ID, 	COL_HEADER_EMPLOYEE_ID },
    };

    const QMap<QString, QString> COLUMN_DATABASE_NAMES = {
        {COL_HEADER_NAME, 			OPL::PilotEntry::NAME},
        {COL_HEADER_ALIAS, 			OPL::PilotEntry::ALIAS},
        {COL_HEADER_COMPANY, 		OPL::PilotEntry::COMPANY},
        {COL_HEADER_EMPLOYEE_ID, 	OPL::PilotEntry::EMPLOYEEID},
    };

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
