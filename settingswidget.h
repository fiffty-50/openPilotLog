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
#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QRegExp>
#include <QValidator>
#include <QMessageBox>
#include <QDebug>
#include "dbsettings.h"

namespace Ui {
class settingsWidget;
}

class settingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit settingsWidget(QWidget *parent = nullptr);
    ~settingsWidget();

private slots:
    void on_flightNumberPrefixLineEdit_textEdited(const QString &arg1);

    void themeGroup_toggled(int id);

private:
    Ui::settingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
