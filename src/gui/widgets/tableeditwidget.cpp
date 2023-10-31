#include "tableeditwidget.h"
#include "src/database/database.h"
#include "src/opl.h"
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

QWidget *TableEditWidget::setupFilterWidget()
{
    // place the filter items in a grid layout so they occupy one cell in parent layout
    QWidget *widget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(widget);

    // one row, three columns
    layout->addWidget(new QLabel(tr("Filter"), this), 0, 0);
    layout->addWidget(filterLineEdit,				  0, 1);
    layout->addWidget(filterSelectionComboBox,		  0, 2);

    return widget;
}

void TableEditWidget::setupSignalsAndSlots()
{
    // refresh the view when the database is updated
    QObject::connect(DB,             		   &OPL::Database::dataBaseUpdated,
                     this,     		 		   &TableEditWidget::databaseContentChanged);
    // filter the view
    QObject::connect(filterLineEdit,  		   &QLineEdit::textChanged,
                     this,                     &TableEditWidget::filterTextChanged);
    // sort the view by column
    QObject::connect(view->horizontalHeader(), &QHeaderView::sectionClicked,
                     this,                     &TableEditWidget::sortColumnChanged);
    // Edit an entry
    QObject::connect(view,					   &QTableView::clicked,
                     this, 			    	   &TableEditWidget::editEntryRequested);
    // Add a new entry
    QObject::connect(addNewEntryPushButton,    &QPushButton::clicked,
                     this, 					   &TableEditWidget::addEntryRequested);
    // Delete a selected entry
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

void TableEditWidget::sortColumnChanged(int newSortColumn)
{
    view->sortByColumn(newSortColumn, Qt::AscendingOrder);
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
