#ifndef EXPORTTOCSVDIALOG_H
#define EXPORTTOCSVDIALOG_H

#include <QDialog>

namespace Ui {
class ExportToCsvDialog;
}

class ExportToCsvDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportToCsvDialog(QWidget *parent = nullptr);
    ~ExportToCsvDialog();

private slots:
    void on_exportPushButton_clicked();

private:
    Ui::ExportToCsvDialog *ui;
    QVector<QVector<QString>> selectedRows;
    const static inline QString exportView = "viewExport";

    void init();
    /*!
     * \brief fill the selectedRows vector with data based on the selected view
     */
    void selectRows();
};

#endif // EXPORTTOCSVDIALOG_H
