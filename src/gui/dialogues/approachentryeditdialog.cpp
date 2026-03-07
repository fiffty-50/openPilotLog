/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#include "approachentryeditdialog.h"
#include "src/database/cache/approachtypeinfo.h"
#include "src/database/database.h"
#include "src/database/entries/approachtypeentry.h"
#include "src/gui/dialogues/entryeditdialog.h"
#include "src/opl.h"
#include <qdialogbuttonbox.h>
#include <qnamespace.h>
#include <qtpreprocessorsupport.h>

ApproachEntryEditDialog::ApproachEntryEditDialog(QWidget *parent)
    : EntryEditDialog(parent), gridLayout(new QGridLayout(this)), lineEdit(new QLineEdit(this)),
      label(new QLabel(this)), buttonBox(new QDialogButtonBox(this))
{
    init();
    retranslateUi();
}

ApproachEntryEditDialog::ApproachEntryEditDialog(int row_id, QWidget *parent)
    : EntryEditDialog(row_id, parent), gridLayout(new QGridLayout(this)), label(new QLabel(this)),
      lineEdit(new QLineEdit(this)), buttonBox(new QDialogButtonBox(this))
{
    init();
    retranslateUi();
    loadEntry(m_rowId);
}

void ApproachEntryEditDialog::init()
{
    buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel |
                                  QDialogButtonBox::StandardButton::Ok);

    int row                     = 0;
    constexpr int firstCol      = 0;
    constexpr int secondCol     = 1;
    constexpr int spanRemaining = -1;

    gridLayout->addWidget(label, row, firstCol);
    gridLayout->addWidget(lineEdit, row, secondCol);
    row++;
    gridLayout->addWidget(buttonBox, row, secondCol, Qt::AlignCenter);

    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this,
                     &ApproachEntryEditDialog::on_accepted);
}

void ApproachEntryEditDialog::retranslateUi() { label->setText(tr("Name")); }

void ApproachEntryEditDialog::loadEntry(int row_id)
{
    m_rowId = row_id;
    lineEdit->setText(approachData->nameFromId(row_id));
}

bool ApproachEntryEditDialog::deleteEntry(int row_id)
{
    return DB->remove(OPL::DbTable::ApproachTypes, row_id);
}

void ApproachEntryEditDialog::reset()
{
    lineEdit->setText({});
    m_rowId = OPL::NEW_ROW_ID;
}

void ApproachEntryEditDialog::on_accepted()
{
    auto entry = OPL::ApproachTypeEntry();
    entry.setRowId(m_rowId);

    if (!entry.setName(lineEdit->text())) {
        WARN(tr("Please enter a valid name for this approach type."));
        return;
    }

    if (!DB->commit(entry)) {
        WARN(tr("Unable to commit. The following error has ocurred: <br><br><b>%1")
                 .arg(DB->lastErrorText()));
        return;
    }
    else {
        QDialog::accept();
    }
}
