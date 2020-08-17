/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void nope();

    void on_newflightButton_clicked();

    void on_actionNew_Flight_triggered();

    void on_actionQuit_triggered();

    void on_quitButton_clicked();

    void on_ShowAllButton_clicked();

    void on_filterComboBox_activated(const QString &arg1);

    void on_editflightButton_clicked();

    void on_deleteFlightPushButton_clicked();

    void on_FilterDateEdit_editingFinished();

    void on_filterFlightsByDateButton_clicked();

    void on_EasaViewButton_clicked();

    void on_tableView_pressed(const QModelIndex &index);

    void on_tableView_entered(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
