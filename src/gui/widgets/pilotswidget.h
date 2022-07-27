/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef PILOTSWIDGET_H
#define PILOTSWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QSqlTableModel>
#include <QTableView>
#include "src/database/row.h"
#include "src/gui/widgets/settingswidget.h"

namespace Ui {
class PilotsWidget;
}
/*!
 * \class PilotsWidget
 * \brief The PilotsWidget is used to view, edit, delete or add new pilots.
 * \details The widget consists of two main parts, a *QTableView* on the left side and a *QStackedWidget* on the right side.
 *
 * In the QTableView, a QSqlTableModel is used to access a view from the database, which holds a Pilots' Last Name,
 * First name and Company.
 *
 * The welcome page shown on the QStackedWidget on the right side has a QLineEdit that functions as a search box and a QCombobox
 * holding the possible columns that can be used to filter what is displayed. The text of the QLineEdit is used as a filter for the
 * QSqlTableModel, so the view is updated in real time.
 *
 * The *NewPilotDialog* is used for creating a new entry as well as for editing an existing entry. If the user selects a row
 * in the QTableView, the NewPilotDialog is displayed on the right side of the Widget, inside the QStackedWidget.
 * In order to avoid leaks from any previously made selections, existing Dialogs are deleted before a new one is created.
 * The NewPilotDialog's `accepted` and `rejected` signals are connected to refresh the view as required.
 *
 * The logbook owner is not shown in the QTableView as an editable Pilot since `self` is a special reserved alias for the
 * pilot with ROWID #1 as a way to identify and adequately display the logbook owner in the logbook. Editing personal details
 * is done via the *SettingsWidget*
 */
class PilotsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PilotsWidget(QWidget *parent = nullptr);
    ~PilotsWidget();

private slots:
    void tableView_selectionChanged();
    void tableView_headerClicked(int);
    void on_newPilotButton_clicked();
    void on_deletePilotButton_clicked();
    void onDeleteUnsuccessful();
    void onNewPilotDialog_editingFinished();
    void on_pilotSearchLineEdit_textChanged(const QString &arg1);

public slots:
    /*!
     * \brief invokes setupModelAndView() to account for changes the user has made in the SettingsWidget
     */
    void onPilotsWidget_settingChanged(SettingsWidget::SettingSignal signal);
    /*!
     * \brief Refreshes the view if the Database has been altered from outside the AircraftWidget
     */
    void onPilotsWidget_databaseUpdated();

    /*!
     * \brief PilotsWidget::repopulateModel (public slot) - re-populates the model to cater for a change
     * to the database connection (for example, when a backup is created)
     */
    void repopulateModel();
private:
    Ui::PilotsWidget *ui;

    QSqlTableModel *model;

    QTableView *view;

    QItemSelectionModel* selectionModel;

    qint32 sortColumn;

    QVector<qint32> selectedPilots;

    const QString getPilotName(const OPL::PilotEntry &pilot);

    const QString getFlightSummary(const OPL::FlightEntry &flight) const;

    void setupModelAndView();

    void connectSignalsAndSlots();

    inline void refreshView(){model->select();}

protected:
    /*!
     * \brief Handles change events, like updating the UI to new localisation
     */
    void changeEvent(QEvent* event) override;
};

#endif // PILOTSWIDGET_H
