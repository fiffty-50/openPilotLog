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
#ifndef AIRCRAFTENTRYEDITDIALOG_H
#define AIRCRAFTENTRYEDITDIALOG_H

#include "entryeditdialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QPlainTextEdit>

class AircraftEntryEditDialog : public EntryEditDialog
{
public:
    explicit AircraftEntryEditDialog(QWidget *parent = nullptr);
    explicit AircraftEntryEditDialog(int row_id, QWidget *parent = nullptr);

    // EntryEditDialog interface
    void loadEntry(int rowID) override;
    bool deleteEntry(int rowID) override;

private:
    void init();
    void retranslateUi();
    void setupSlots();
    void loadAircraftData(int rowId);

    QGridLayout *gridLayout;
    QLabel *makeLabel;
    QLineEdit *makeLineEdit;
    QLabel *modelLabel;
    QLineEdit *modelLineEdit;
    QLabel *variantLabel;
    QLineEdit *variantLineEdit;
    QLabel *icaoDesignatorLabel;
    QLineEdit *icaoDesignatorLineEdit;
    QLabel *engineTypeLabel;
    QComboBox *engineTypeComboBox;
    QLabel *multiEngineLabel;
    QComboBox *multiEngineComboBox;
    QLabel *multiPilotLabel;
    QComboBox *multiPilotComboBox;
    QLabel *classLabel;
    QComboBox *classComboBox;
    QLabel *subClassLabel;
    QComboBox *subClassComboBox;
    QLabel *wakeCategoryLabel;
    QComboBox *wakeCategoryComboBox;
    QLabel *typeRatingLabel;
    QLineEdit *typeRatingLineEdit;
    QLabel *remarksLabel;
    QPlainTextEdit *remarksTextEdit;
    QDialogButtonBox *buttonBox;

private slots:
    void on_accepted();
};

#endif // AIRCRAFTENTRYEDITDIALOG_H
