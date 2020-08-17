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
#ifndef EDITFLIGHT_H
#define EDITFLIGHT_H

#include <QDialog>

namespace Ui {
class EditFlight;
}

class EditFlight : public QDialog
{
    Q_OBJECT

public:
    explicit EditFlight(QWidget *parent = nullptr);
    ~EditFlight();

private slots:
    void nope();//error box

    QVector<QString> collectInput();

    bool verifyInput();

    void returnInput(QVector<QString> flight);

    void on_verifyButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_newDept_editingFinished();

    void on_newDoft_editingFinished();

    void on_newTofb_editingFinished();

    void on_newDest_editingFinished();

    void on_newTonb_editingFinished();

    void on_newAcft_editingFinished();

    void on_newPic_editingFinished();

    void on_newDept_textChanged(const QString &arg1);

    void on_newDest_textChanged(const QString &arg1);

    void on_newAcft_textChanged(const QString &arg1);

    void on_newPic_textChanged(const QString &arg1);
    
    
private:
    Ui::EditFlight *ui;
};

#endif // EDITFLIGHT_H
