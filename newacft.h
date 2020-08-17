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
#ifndef NEWACFT_H
#define NEWACFT_H

#include <QDialog>

namespace Ui {
class NewAcft;
}

class NewAcft : public QDialog
{
    Q_OBJECT

public:
    explicit NewAcft(QWidget *parent = nullptr);
    ~NewAcft();

private slots:

    void on_MakeLineEdit_textEdited(const QString &arg1);

    void on_ModelLineEdit_textEdited(const QString &arg1);

    void on_VariantLineEdit_textEdited(const QString &arg1);

    void on_MakeLineEdit_editingFinished();

    void on_ModelLineEdit_editingFinished();

    void on_VariantLineEdit_editingFinished();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_VerifyButton_clicked();

    void on_RegistrationLineEdit_editingFinished();


    void on_EasaEnabledCheckBox_stateChanged();

    void on_showAllPushButton_clicked();

private:
    Ui::NewAcft *ui;
};

#endif // NEWACFT_H
