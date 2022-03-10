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
#ifndef LOGBOOKWIDGET_H
#define LOGBOOKWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QDebug>
#include <QMenu>
#include <QTableView>
#include "src/gui/widgets/settingswidget.h"
#include "src/classes/acompletiondata.h"
#include "src/opl.h"

namespace Ui {
class LogbookWidget;
}

/*!
 * \brief The LogbookWidget displays data from the database in a QSqlTableView fed by a QSqlQuery Model
 *
 * \details The LogbookWidget is the primary display interface for flights logged in the database. It fetches and stores
 * flight data from the database via a QSqlQueryModel and displays it in a QTableView. With the way the flight data is
 * written in the database, it would not be human-readable, so some processing is done on the database side to present
 * a nicely formatted, human-readable display. This is achieved by means of a [SQL View](https://sqlite.org/lang_createview.html).
 *
 * The user can select a view from a list of available views in the SettingsWidget.
 *
 */
class LogbookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogbookWidget(ACompletionData &completion_data, QWidget *parent = nullptr);
    ~LogbookWidget();

private slots:
    void flightsTableView_selectionChanged();
    void on_tableView_customContextMenuRequested(const QPoint &pos);
    void on_actionDelete_Flight_triggered();

    void on_tableView_doubleClicked();
    void on_flightSearchLlineEdit_textChanged(const QString &arg1);
    void on_flightSearchComboBox_currentIndexChanged(int);

    void on_viewsComboBox_currentIndexChanged(int index);

    void on_actionEdit_Flight_triggered();
    void on_actionEdit_Sim_triggered();

public slots:
    void refresh();
    void onLogbookWidget_viewSelectionChanged(SettingsWidget::SettingSignal signal);
    void repopulateModel();

private:
    Ui::LogbookWidget *ui;

    QTableView* view;

    QSqlTableModel* displayModel;

    QItemSelectionModel* selectionModel;

    QMenu* menu;

    QVector<qint32> selectedEntries;

    void setupModelAndView(int view_id);
    void connectSignalsAndSlots();

    ACompletionData completionData;

    /*!
     * \brief isFlight Determines whether an entry shown in a view is a Flight or a Simulator.
     * \param model_row_id the row id in the QSqlTableModel used for displaying
     * \details In the composite views (SQL UNION) with Simulators included, the row_id of the
     * simulator entries is inverted to a negative value. A positive row id is thus a row id from
     * the flights table, whereas a negative rowid is a row id from the simulators table.
     */
    inline bool isFlight(int model_row_id) { return model_row_id > 0; }

protected:
    /*!
     * \brief Handles change events, like updating the UI to new localisation
     */
    void changeEvent(QEvent* event) override;
};

#endif // LOGBOOKWIDGET_H
