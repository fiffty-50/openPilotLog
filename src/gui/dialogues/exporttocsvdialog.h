#ifndef EXPORTTOCSVDIALOG_H
#define EXPORTTOCSVDIALOG_H

#include <QDialog>

namespace Ui {
class ExportToCsvDialog;
}

/*!
 * \brief The ExportToCsvDialog class enables the user to export the database to a CSV file
 */
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
     * \brief fill the selectedRows vector with data based on the selected view.
     * The selected rows are a subset of the rows available in the database.
     */
    void selectRows();
};

#endif // EXPORTTOCSVDIALOG_H
