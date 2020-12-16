#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include "src/database/db.h"
#include "src/database/adatabasesetup.h"
#include "src/database/dbinfo.h"
#include "src/classes/adownload.h"
#include "src/functions/areadcsv.h"

#include "src/experimental/adatabase.h"
#include "src/experimental/aentry.h"
#include "src/experimental/apilotentry.h"
#include "src/experimental/expnewflightdialog.h"

#include "src/testing/abenchmark.h"
#include "src/testing/atimer.h"
#include "src/testing/adebug.h"

namespace Ui {
class DebugWidget;
}

class DebugWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWidget(QWidget *parent = nullptr);
    ~DebugWidget();

private slots:
    void on_resetUserTablesPushButton_clicked();

    void on_resetDatabasePushButton_clicked();

    void downloadFinished();

    void on_fillUserDataPushButton_clicked();

    void on_selectCsvPushButton_clicked();

    void on_importCsvPushButton_clicked();

    void on_debugPushButton_clicked();

private:
    Ui::DebugWidget *ui;

    bool downloadComplete = false;
};

#endif // DEBUGWIDGET_H
