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
#include "totalswidget.h"
#include "ui_totalswidget.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"

TotalsWidget::TotalsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TotalsWidget)
{
    ui->setupUi(this);
    auto data = AStat::totals();
    DEB << "Filling Totals Line Edits...";
    for (const auto &field : data) {
        auto line_edit = parent->findChild<QLineEdit *>(field.first + "LineEdit");
        line_edit->setText(field.second);
    }

    QString salutation = tr("Welcome to openPilotLog");
    salutation.append(QStringLiteral(", ") + userName() + QLatin1Char('!'));

    ui->welcomeLabel->setText(salutation);
}

TotalsWidget::~TotalsWidget()
{
    delete ui;
}

const QString TotalsWidget::userName()
{
    auto namestring = aDB->getPilotEntry(1).name();
    auto names = namestring.split(',');
    if (!names.isEmpty())
        return names[0];

    return QString();
}
