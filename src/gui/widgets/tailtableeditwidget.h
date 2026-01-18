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


    void setupModelAndView() override;
    void setupUI() override;
    QString deleteErrorString(int rowId) override;
    QString confirmDeleteString(int rowId) override;
    EntryEditDialog *getEntryEditDialog(QWidget *parent) override;

private:
    static constexpr int COL_ROWID = 0;
    static constexpr int COL_REGISTRATION = 1;
    static constexpr int COL_TYPE = 2;
    static constexpr int COL_COMPANY = 3;

    const QString COLUMN_NAME_REGISTRATION = tr("Registration");
    const QString COLUMN_NAME_TYPE = tr("Type");
    const QString COLUMN_NAME_COMPANY = tr("Company");

    const QMap<int, QString> COLUMN_HEADERS_MAP = {
        {COL_REGISTRATION, COLUMN_NAME_REGISTRATION },
        {COL_TYPE, COLUMN_NAME_TYPE },
        {COL_COMPANY, COLUMN_NAME_COMPANY },
    };
    
    const QList<int> HIDDEN_COLUMNS = {
        COL_ROWID,
    };

    const QList<int> VISIBLE_COLUMNS = {
        COL_REGISTRATION,
        COL_TYPE,
        COL_COMPANY,
    };

    const QList<int>* getHiddenColumns() const override { return &HIDDEN_COLUMNS; }
    const QList<int>* getVisibleColumns() const override { return &VISIBLE_COLUMNS; }
    const QMap<int, QString>* getColumnHeaderMap() const override { return &COLUMN_HEADERS_MAP; }
};

#endif // TAILTABLEEDITWIDGET_H
