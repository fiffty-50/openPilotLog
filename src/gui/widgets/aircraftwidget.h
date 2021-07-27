/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#ifndef AIRCRAFTWIDGET_H
#define AIRCRAFTWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QSqlTableModel>
#include <QTableView>
#include "src/gui/widgets/settingswidget.h"


namespace Ui {
class AircraftWidget;
}
/*!
 * \class AircraftWidget
 * \brief The AircraftWidget is used to view, edit, delete or add new tails.
 * \details The widget consists of two main parts, a *QTableView* on the left side and a *QStackedWidget* on the right side.
 *
 * In the QTableView, a QSqlTableModel is used to access a view from the database, which holds a tails' Registration, Type and
 * Company.
 *
 * The welcome page shown on the QStackedWidget on the right side has a QLineEdit that functions as a search box and a QCombobox
 * holding the possible columns that can be used to filter what is displayed. The text of the QLineEdit is used as a filter for the
 * QSqlTableModel, so the view is updated in real time.
 *
 * The *NewTailDialog* is used for creating a new entry as well as for editing an existing entry. If the user selects a row
 * in the QTableView, the NewTailDilog is displayed on the right side of the Widget, inside the QStackedWidget.
 * In order to avoid leaks from any previously made selections, existing Dialogs are deleted before a new one is created.
 * The NewTailDialog's `accepted` and `rejected` signals are connected to refresh the view as required.
 *
 * Note: The ATailEntry class is used to operate on individual aircraft, whereas the AAircraftEntry class is used to retreive
 * templates of aircraft types. For example, 'D-ABCD' and 'N-XYZ' are different tails (Registrations), but they might be the same type of aircraft,
 * for example 'Boeing 737-800'.
 */
class AircraftWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AircraftWidget(QWidget *parent = nullptr);
    ~AircraftWidget();

private slots:
    void tableView_selectionChanged();

    void tableView_headerClicked(int column);

    void on_deleteAircraftButton_clicked();

    void on_newAircraftButton_clicked();

    void onNewTailDialog_editingFinished();

    void on_aircraftSearchLineEdit_textChanged(const QString &arg1);

public slots:
    /*!
     * \brief invokes setupModelAndView() to account for changes the user has made in the SettingsWidget
     */
    void onAircraftWidget_settingChanged(SettingsWidget::SettingSignal signal);
    /*!
     * \brief Refreshes the view if the Database has been altered from outside the AircraftWidget
     */
    void onAircraftWidget_dataBaseUpdated();

    /*!
     * \brief AircraftWidget::repopulateModel (public slot) - re-populates the model to cater for a change
     * to the database connection (for example, when a backup is created)
     */
    void repopulateModel();
private:
    Ui::AircraftWidget *ui;

    QSqlTableModel *model;

    QTableView *view;

    QItemSelectionModel* selection;

    qint32 sortColumn;

    /*!
     * \brief selectedTails Holds a list of the entries the user has selected
     */
    QVector<qint32> selectedTails;

    void setupModelAndView();

    void connectSignalsAndSlots();

    void onDeleteUnsuccessful();

    inline void refreshView(){model->select();}

protected:
    /*!
     * \brief Handles change events, like updating the UI to new localisation
     */
    void changeEvent(QEvent* event) override;
};

#endif // AIRCRAFTWIDGET_H
