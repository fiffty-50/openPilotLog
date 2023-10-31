#include "tableeditwidget.h"
#include "src/database/database.h"
#include "src/opl.h"
#include "src/classes/settings.h"
#include <QGridLayout>
#include <QLabel>

TableEditWidget::TableEditWidget(QWidget *parent)
    : QWidget{parent}
{}

void TableEditWidget::init() {
    setupUI();
    setupSignalsAndSlots();
}

void TableEditWidget::setupUI()
{
    // Setting up the model and view is done in the derived class
    setupModelAndView();

    // Set up the editing widget
    stackedWidget->hide();

    // create a 2-column grid layout and fill the cells
    int colL = 0; // left column
    int colR = 1; // right column
    int row = 0;
    int allRowSpan = 4; // adjust as needed for stackedWidget to span all rows

    auto gridLayout = new QGridLayout(this);

    gridLayout->addWidget(view, row, colL);
    gridLayout->addWidget(stackedWidget, row, colR, allRowSpan, 1);
    row++;

    gridLayout->addWidget(addNewEntryPushButton, row, colL);
    row++;

    gridLayout->addWidget(deleteEntryPushButton, row, colL);
    row++;

    gridLayout->addWidget(setupFilterWidget(), row, colL);
}

void TableEditWidget::setupSignalsAndSlots()
{
    QObject::connect(DB,             		   &OPL::Database::dataBaseUpdated,
                     this,     		 		   &TableEditWidget::databaseContentChanged);
    QObject::connect(filterLineEdit,  		   &QLineEdit::textChanged,
                     this,                     &TableEditWidget::filterTextChanged);
    QObject::connect(view->horizontalHeader(), &QHeaderView::sectionClicked,
                     this,                     &TableEditWidget::sortColumnChanged);
    QObject::connect(view,					   &QTableView::clicked,
                     this, 			    	   &TableEditWidget::editEntryRequested);
    QObject::connect(addNewEntryPushButton,    &QPushButton::clicked,
                     this, 					   &TableEditWidget::addEntryRequested);
    QObject::connect(deleteEntryPushButton,    &QPushButton::clicked,
                     this, 					   &TableEditWidget::deleteEntryRequested);
}

void TableEditWidget::addEntryRequested()
{
    clearStackedWidget();

    // create a Dialog for adding a new entry and put it on the stackedWidget
    auto editDialog = getEntryEditDialog(this);
    stackedWidget->addWidget(editDialog);
    stackedWidget->show();
    editDialog->exec();

    stackedWidget->hide();

}


void TableEditWidget::editEntryRequested(const QModelIndex &selectedIndex)
{
    clearStackedWidget();

    // create a Dialog for editing the selected entry and put it on the stackedWidget
    int rowId = model->index(selectedIndex.row(), 0).data().toInt();
    auto editEntryDialog = getEntryEditDialog(this);
    editEntryDialog->loadEntry(rowId);

    stackedWidget->addWidget(editEntryDialog);
    stackedWidget->setCurrentWidget(editEntryDialog);
    stackedWidget->show();
    editEntryDialog->exec();

    stackedWidget->hide();

}

void TableEditWidget::deleteEntryRequested()
{
    const QModelIndex selectedIndex = view->selectionModel()->currentIndex();
    if(!selectedIndex.isValid()) {
        WARN(tr("No entry selected."));
        return;
    }

    stackedWidget->hide();
    clearStackedWidget();
    int rowId = model->index(selectedIndex.row(), 0).data().toInt();
    view->selectionModel()->reset();

    // get user confirmation
    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(tr("Confirm Deletion"));

    confirm.setText(confirmDeleteString(rowId));
    if (confirm.exec() == QMessageBox::Yes) {
        auto editDialog = getEntryEditDialog(this);
        if(!editDialog->deleteEntry(rowId))
            WARN(deleteErrorString(rowId));
    }
}

void TableEditWidget::filterTextChanged(const QString &filterString)
{
    TODO << "Create map <index, column Name> and implement func in derived";
    model->setFilter(QLatin1Char('\"') + filterSelectionComboBox->currentText()
                     + QLatin1String("\" LIKE '%") + filterString
                     + QLatin1String("%' AND ID > 1"));
}

void TableEditWidget::sortColumnChanged(int newSortColumn)
{
    view->sortByColumn(newSortColumn, Qt::AscendingOrder);
    Settings::setPilotSortColumn(newSortColumn);
}

QWidget *TableEditWidget::setupFilterWidget()
{
    QWidget *widget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(widget);

    int row =  0;
    int colL = 0;
    int colM = 1;
    int colR = 2;

    layout->addWidget(new QLabel(tr("Filter"), this), row, colL);
    layout->addWidget(filterLineEdit, row, colM);
    layout->addWidget(filterSelectionComboBox, row, colR);

    return widget;
}

void TableEditWidget::clearStackedWidget()
{
    while (stackedWidget->count() > 0) {
        QWidget *orphan = stackedWidget->currentWidget();
        stackedWidget->removeWidget(orphan);
        delete orphan;
    }
}

void TableEditWidget::databaseContentChanged()
{
    model->select();
    view->resizeColumnsToContents();
}
