#include "exporttocsvdialog.h"
#include "QtSql/qsqlfield.h"
#include "QtSql/qsqlrecord.h"
#include "QtSql/qsqltablemodel.h"
#include "QtWidgets/qfiledialog.h"
#include "src/opl.h"
#include "ui_exporttocsvdialog.h"
#include "src/functions/readcsv.h"

ExportToCsvDialog::ExportToCsvDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportToCsvDialog)
{
    ui->setupUi(this);
    init();
}

ExportToCsvDialog::~ExportToCsvDialog()
{
    delete ui;
}

void ExportToCsvDialog::on_exportPushButton_clicked()
{
    selectRows();
    // File Dialog where to save
    QString filePath = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this,
                                                    tr("Select Location"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                                             QStringLiteral("*.csv")));
    if(filePath.isEmpty()) return; // user has cancelled file dialog

    if(!filePath.endsWith(QStringLiteral(".csv")))
        filePath += ".csv";
    DEB << filePath;
    if(!selectedRows.isEmpty()) {
        if(CSV::writeCsv(filePath, selectedRows)) {
            INFO("Database successfully exported.");
            QDialog::accept();
        } else {
            WARN("Unable to save csv file.");
        }
    }
}

void ExportToCsvDialog::init()
{
    OPL::GLOBALS->fillViewNamesComboBox(ui->viewComboBox);
    ui->viewComboBox->addItem(tr("Default - Export"));
    ui->viewComboBox->setCurrentIndex(5);
}

void ExportToCsvDialog::selectRows()
{
    selectedRows.clear();
    // create a QSqlTableModel based on the selected views
    const auto model = new QSqlTableModel(this);
    if(ui->viewComboBox->currentIndex() < 4)
        model->setTable(OPL::GLOBALS->getViewIdentifier(OPL::DbViewName(ui->viewComboBox->currentIndex())));
    else
        model->setTable(exportView);
    model->select();

    int columns = model->record().count();
    // check model is usable
    if(model->rowCount() == 0 || columns == 0) {
        WARN("Invalid Model, unable to parse CSV");
        return;
    }
    QVector<QString> newRow;

    // extract headers
    for(int i = 0; i < columns; i++)
        newRow.append(model->headerData(i, Qt::Horizontal).toString());
    selectedRows.append(newRow);
    newRow.clear();

    // add column data
    for(int i = 0; i < model->rowCount(); i++) {
        const auto record = model->record(i);
        for(int i = 0; i < columns; i++)
            newRow.append(record.field(i).value().toString());
        selectedRows.append(newRow);
        newRow.clear();
    }
}

