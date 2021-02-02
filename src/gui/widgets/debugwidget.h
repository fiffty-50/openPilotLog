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
#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include "src/database/adatabasesetup.h"
#include "src/classes/adownload.h"
#include "src/functions/areadcsv.h"

#include "src/database/adatabase.h"
#include "src/classes/aentry.h"
#include "src/classes/apilotentry.h"

#include "src/testing/abenchmark.h"
#include "src/testing/atimer.h"
#include "src/testing/adebug.h"

namespace Ui {
class DebugWidget;
}

class DebugWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWidget(QWidget *parent = nullptr);
    ~DebugWidget();

private slots:
    void on_resetUserTablesPushButton_clicked();

    void on_resetDatabasePushButton_clicked();

    void downloadFinished();

    void on_fillUserDataPushButton_clicked();

    void on_selectCsvPushButton_clicked();

    void on_importCsvPushButton_clicked();

    void on_debugPushButton_clicked();

private:
    Ui::DebugWidget *ui;

    bool downloadComplete = false;
};

#endif // DEBUGWIDGET_H
