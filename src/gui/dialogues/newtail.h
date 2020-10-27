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
#ifndef NEWTAIL_H
#define NEWTAIL_H

#include <QDialog>
#include <QCompleter>
#include "src/classes/completionlist.h"

namespace Ui {
class NewTail;
}

class NewTail : public QDialog
{
    Q_OBJECT

public:
    explicit NewTail(QWidget *parent = nullptr);
    ~NewTail();

private slots:

    void on_searchLineEdit_textChanged(const QString &arg1);

private:
    Ui::NewTail *ui;

    QStringList aircraftlist;
};

#endif // NEWTAIL_H