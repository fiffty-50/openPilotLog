#ifndef LOGBOOKTABLEEDITWIDGET_H
#define LOGBOOKTABLEEDITWIDGET_H

#include "tableeditwidget.h"
#include "src/opl.h"
#include <QObject>

/*!
 * \brief The LogbookTableEditWidget allows editing of logbook entries.
 *
 * \details This widget differs from the other TableEditWidget implementations in that
 * instead of a table, it uses a [view](https://sqlite.org/lang_createview.html) as the source of the display model.
 *
 * The user can select a view from a list of available views in the SettingsWidget.
 *
 * Some of the selectable views aggregate data from the flights and simulators table. In those views, flight
 * entries have positive row id's whereas the simulators have inverse (negative) row id's.
 */
class LogbookTableEditWidget : public TableEditWidget
{
    Q_OBJECT
public:
    LogbookTableEditWidget() = delete;
    explicit LogbookTableEditWidget(QWidget *parent = nullptr);

private:
    OPL::LogbookView m_logbookView;
    void setupDelegates();

    static constexpr int COL_ROWID = 0;

    // TableEditWidget interface
public:
    virtual void setupModelAndView() override;
    virtual void setupUI() override;
    virtual QString deleteErrorString(int rowId) override;
    virtual QString confirmDeleteString(int rowId) override;
    virtual EntryEditDialog *getEntryEditDialog(QWidget *parent) override;

private slots:
    virtual void filterTextChanged(const QString &filterString) override;
};

#endif // LOGBOOKTABLEEDITWIDGET_H
