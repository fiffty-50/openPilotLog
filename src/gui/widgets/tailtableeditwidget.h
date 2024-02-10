#ifndef TAILTABLEEDITWIDGET_H
#define TAILTABLEEDITWIDGET_H

#include "tableeditwidget.h"
#include "src/database/tailentry.h"

class TailTableEditWidget : public TableEditWidget
{
    Q_OBJECT
public:
    TailTableEditWidget() = delete;
    explicit TailTableEditWidget(QWidget *parent = nullptr);


    virtual void setupModelAndView() override;
    virtual void setupUI() override;
    virtual QString deleteErrorString(int rowId) override;
    virtual QString confirmDeleteString(int rowId) override;
    virtual EntryEditDialog *getEntryEditDialog(QWidget *parent) override;

private:
    const int COL_ROWID = 0;
    const int COL_REGISTRATION = 1;
    const int COL_TYPE = 10;
    const int COL_COMPANY = 2;

    const int COLS_TO_HIDE[8] = {0, 3, 4, 5, 6, 7, 8, 9};

    const QString COLUMN_NAME_REGISTRATION = tr("Registration");
    const QString COLUMN_NAME_TYPE = tr("Type");
    const QString COLUMN_NAME_COMPANY = tr("Company");

    const QStringList FILTER_COLUMNS = {
        COLUMN_NAME_REGISTRATION,
        COLUMN_NAME_TYPE,
        COLUMN_NAME_COMPANY,
    };

    const static inline QStringList FILTER_COLUMN_NAMES = {
        OPL::TailEntry::REGISTRATION,
        OPL::TailEntry::TYPE_STRING,
        OPL::TailEntry::COMPANY
    };

private slots:

    virtual void filterTextChanged(const QString &filterString) override;
};

#endif // TAILTABLEEDITWIDGET_H
