/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "homewidget.h"
#include "src/gui/widgets/currencywidget.h"
#include "src/gui/widgets/totalswidget.h"
#include "ui_homewidget.h"
#include "src/database/database.h"

HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);

    const auto logo = QPixmap(OPL::Assets::LOGO);
    ui->logoLabel->setPixmap(logo);
    ui->welcomeLabel->setText(tr("Welcome to openPilotLog, %1!").arg(getLogbookOwnerName()));

    fillTotals();
    fillCurrencies();

    QObject::connect(DB,    &OPL::Database::dataBaseUpdated,
                     this,  &HomeWidget::onPilotsDatabaseChanged);
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::fillTotals()
{
    auto tw = new TotalsWidget(TotalsWidget::TotalTimeWidget, this);
    ui->tabWidget->insertTab(0, tw, tr("Totals"));
}

void HomeWidget::fillCurrencies()
{
    auto cw = new CurrencyWidget(this);
    ui->tabWidget->insertTab(1, cw, tr("Currencies"));
}

void HomeWidget::onPilotsDatabaseChanged(const OPL::DbTable table)
{
    if (table == OPL::DbTable::Pilots)
        ui->welcomeLabel->setText(tr("Welcome to openPilotLog, %1!").arg(getLogbookOwnerName()));
}

void HomeWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
}

const QString HomeWidget::getLogbookOwnerName() const
{
    OPL::PilotEntry owner = DB->getLogbookOwner();
    QString name = owner.getFirstName();
    if(name.isEmpty()) {
        name = owner.getLastName();
    }
    return name;
}
