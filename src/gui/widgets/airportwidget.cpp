#include "airportwidget.h"
#include "ui_airportwidget.h"
#include "src/gui/dialogues/newairportdialog.h"
#include "src/database/database.h"

AirportWidget::AirportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AirportWidget)
{
    ui->setupUi(this);
    setupModelAndeView();
    setupSearch();

    QObject::connect(model,                  &QSqlTableModel::beforeUpdate,
                     this,                   &AirportWidget::onUpdate);
    QObject::connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this,                   &AirportWidget::onSelectionChanged);
}

AirportWidget::~AirportWidget()
{
    delete ui;
}

void AirportWidget::setupModelAndeView()
{
    model = new QSqlTableModel(this);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setTable(TABLE_NAME);
    model->select();
    model->sort(1, Qt::AscendingOrder);

    view = ui->tableView;
    view->setModel(model);

    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->hideColumn(0);
    view->resizeColumnsToContents();
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->show();
}

void AirportWidget::setupSearch()
{
    for (const auto &item : HEADER_MAP)
        ui->searchComboBox->addItem(item);
}

void AirportWidget::on_searchLineEdit_textChanged(const QString &arg1)
{
    if(arg1.length() == 0) {
        model->setFilter("");
        model->select();
        return;
    }

    model->setFilter(FILTER_MAP.value(ui->searchComboBox->currentIndex())
                     + arg1 + QLatin1String("%\""));
}


void AirportWidget::on_searchComboBox_currentIndexChanged(int index)
{
    ui->searchLineEdit->setText(QString());
}

void AirportWidget::on_newAirportPushButton_clicked()
{
    auto ap_dialog = NewAirportDialog(this);
    if (ap_dialog.exec() == QDialog::Accepted) {
        model->select();
        emit DB->dataBaseUpdated(OPL::DbTable::Airports);
    }
}

void AirportWidget::on_deletePushButton_clicked()
{
    DEB << "Airports selected: " << selectedEntries.length();
    if (selectedEntries.length() == 0) {
        WARN(tr("<br>No Airport selected.<br>"));
        return;
    } else if (selectedEntries.length() > 0 && selectedEntries.length() <= 10) {
        QStringList selected_airport_names;
        for (const auto row_id : qAsConst(selectedEntries)) {
            const auto data = DB->getRowData(OPL::DbTable::Airports, row_id);
            selected_airport_names.append(data.value(OPL::Db::AIRPORTS_NAME).toString());
        }

        QString selected_airports_string;

        for (auto &name : selected_airport_names) {
            selected_airports_string.append(name);
            selected_airports_string.append(QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
        }
        QMessageBox confirm(this);
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Question);
        confirm.setWindowTitle("Delete Airport");
        confirm.setText(tr("The following airport(s) will be deleted:<br><br><b><tt>"
                           "%1<br></b></tt>"
                           "Deleting airports is irreversible.<br>Do you want to proceed?"
                           ).arg(selected_airports_string));
        if (confirm.exec() == QMessageBox::Yes) {
            for (auto& row_id : selectedEntries) {
                if(!DB->remove(OPL::Row(OPL::DbTable::Airports, row_id))) {
                    WARN(tr("<br>Unable to delete.<br><br>The following error has ocurred: %1"
                                       ).arg(DB->lastError.text()));
                    return;
                }
            }
            INFO(tr("%1 Airports have been deleted successfully."
                                   ).arg(QString::number(selectedEntries.length())));
            model->select();
        }
    } else if (selectedEntries.length() > 10) {
        QMessageBox confirm;
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Warning);
        confirm.setWindowTitle("Delete Airports");
        confirm.setText(tr("You have selected %1 airports.<br><br>"
                           "Deleting airports is irreversible.<br><br>"
                           "Are you sure you want to proceed?"
                           ).arg(QString::number(selectedEntries.length())));
        if(confirm.exec() == QMessageBox::Yes) {
            if (!DB->removeMany(OPL::DbTable::Airports, selectedEntries)) {
                WARN(tr("Unable to delete. No changes have been made to the database. The following error has ocurred:<br><br>%1").arg(DB->lastError.text()));
                return;
            }
            INFO(tr("%1 Airports have been deleted successfully."
                                   ).arg(QString::number(selectedEntries.length())));
            model->select();
        }
        model->select();
    }
}

void AirportWidget::onUpdate()
{
    emit DB->dataBaseUpdated(OPL::DbTable::Airports);
}

void AirportWidget::onSelectionChanged()
{
    selectedEntries.clear();
    for (const auto& row : view->selectionModel()->selectedRows()) {
        selectedEntries.append(row.data().toInt());
        DEB << "Selected Airport(s) with ID: " << selectedEntries;
    }
}



