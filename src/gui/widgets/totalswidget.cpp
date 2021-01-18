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
#include "totalswidget.h"
#include "ui_totalswidget.h"
#include "src/testing/adebug.h"

TotalsWidget::TotalsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TotalsWidget)
{
    ui->setupUi(this);
    auto data = AStat::totals();
    DEB << "Filling Totals Line Edits...";
    //DEB << "data: " << data;
    for (const auto &field : data) {
        auto line_edit = parent->findChild<QLineEdit *>(field.first + "LineEdit");
        line_edit->setText(field.second);
    }
    QSettings settings;
    QString name = settings.value("userdata/firstname").toString();
    if(!name.isEmpty()) {
        QString salutation = "Welcome to openPilotLog, " + name + QLatin1Char('!');
        ui->welcomeLabel->setText(salutation);
    }
}

TotalsWidget::~TotalsWidget()
{
    delete ui;
}
