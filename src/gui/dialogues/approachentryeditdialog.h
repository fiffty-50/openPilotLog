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
#ifndef APPROACHENTRYEDITDIALOG_H
#define APPROACHENTRYEDITDIALOG_H

#include "entryeditdialog.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QObject>
#include <QLabel>

class ApproachEntryEditDialog : public EntryEditDialog {
    Q_OBJECT
  public:
    explicit ApproachEntryEditDialog(QWidget *parent = nullptr);
    explicit ApproachEntryEditDialog(int row_id, QWidget *parent = nullptr);

    void loadEntry(int rowID) override;
    bool deleteEntry(int rowID) override;
    void reset() override;

  private:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QDialogButtonBox *buttonBox;

    void init();
    void retranslateUi();

  private slots:
    void on_accepted();
};

#endif // APPROACHENTRYEDITDIALOG_H
