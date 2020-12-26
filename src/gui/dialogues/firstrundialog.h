#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QMessageBox>
#include "src/classes/asettings.h"
#include "src/testing/adebug.h"
#include "src/experimental/apilotentry.h"
#include "src/experimental/adatabase.h"
#include "src/classes/adownload.h"
#include "src/database/adatabasesetup.h"

namespace Ui {
class FirstRunDialog;
}

class FirstRunDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FirstRunDialog(QWidget *parent = nullptr);
    ~FirstRunDialog();

private slots:

    void on_previousPushButton_clicked();

    void on_nextPushButton_clicked();

    void on_themeGroup_toggled(int id);

    void on_finishButton_clicked();

    bool finishSetup();

private:
    Ui::FirstRunDialog *ui;

};

#endif // FIRSTRUNDIALOG_H
