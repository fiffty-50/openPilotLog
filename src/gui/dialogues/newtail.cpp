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
#include "newtail.h"
#include "ui_newtail.h"

NewTail::NewTail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    ui->setupUi(this);

    auto cl = new completionList(completerTarget::aircraft);
    aircraftlist = cl->list;

    QCompleter* completer = new QCompleter(aircraftlist,ui->searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->searchLineEdit->setCompleter(completer);

}

NewTail::~NewTail()
{
    delete ui;
}


void NewTail::on_searchLineEdit_textChanged(const QString &arg1)
{
    if(aircraftlist.contains(arg1)){
        qDebug() << "Success!" << ui->searchLineEdit->text();
        //call autofiller for dialog
        ui->searchLineEdit->setStyleSheet("border: 1px solid green");
    }else{
        ui->searchLineEdit->setStyleSheet("border: 1px solid orange");
    }
}
