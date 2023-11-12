#ifndef LOGBOOKTABLEEDITWIDGET_H
#define LOGBOOKTABLEEDITWIDGET_H

#include "settingswidget.h"
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
 * entries have positive row id's whereas the simulators have inverse (negative) row id's. This necessitates
 * checking the row id's value before deciding on the apropriate EntryEditDialog.
 */
class LogbookTableEditWidget : public TableEditWidget
{
    Q_OBJECT
public:
    LogbookTableEditWidget() = delete;
    explicit LogbookTableEditWidget(QWidget *parent = nullptr);

private:
    OPL::LogbookView m_logbookView;
    OPL::DateTimeFormat m_format;

    /*!
     * \brief Set up the QStyledItemDelegate instances that transform the database values to user-readable values.
     */
    void setupDelegates();

    static constexpr int COL_ROWID = 0;

    // keep track of default and custom delegates set on certain columns
    QHash<int, QAbstractItemDelegate*> m_defaultDelegates;

    // TableEditWidget interface
public:
    virtual void setupModelAndView() override;
    virtual void setupUI() override;
    virtual QString deleteErrorString(int rowId) override;
    virtual QString confirmDeleteString(int rowId) override;
    virtual EntryEditDialog *getEntryEditDialog(QWidget *parent) override;

public slots:
    void viewSelectionChanged(SettingsWidget::SettingSignal widget);
private slots:
    virtual void filterTextChanged(const QString &filterString) override;
    virtual void addEntryRequested() override;
    virtual void editEntryRequested(const QModelIndex &selectedIndex) override;
    virtual void deleteEntryRequested() override;
};

#endif // LOGBOOKTABLEEDITWIDGET_H
