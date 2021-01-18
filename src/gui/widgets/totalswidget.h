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
#ifndef TOTALSWIDGET_H
#define TOTALSWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include "src/functions/astat.h"

namespace Ui {
class TotalsWidget;
}

class TotalsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TotalsWidget(QWidget *parent = nullptr);
    ~TotalsWidget();

private:
    Ui::TotalsWidget *ui;
};

#endif // TOTALSWIDGET_H
