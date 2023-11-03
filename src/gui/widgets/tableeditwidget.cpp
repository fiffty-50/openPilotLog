#include "tableeditwidget.h"
#include "src/database/database.h"
#include "src/opl.h"
#include <QGridLayout>
#include <QLabel>

TableEditWidget::TableEditWidget(Orientation orientation, QWidget *parent)
    : QWidget{parent}, _orientation(orientation)
{}

void TableEditWidget::init() {
    setupUI();
    setupSignalsAndSlots();
}

void TableEditWidget::setupUI()
{
    // Setting up the model and view is done in the derived class
    setupModelAndView();
    _entryEditDialog = getEntryEditDialog(this);
    stackedWidget->addWidget(_entryEditDialog);

    // set up the UI
    switch (_orientation) {
    case Horizontal:
        setupHorizontalUI();
        break;
    case Vertical:
        setupVerticalUI();
    default:
        break;
    }

}

void TableEditWidget::setupHorizontalUI()
{
    // In the horizontal view, the editing widget is hidden on the right hand side
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

    setupButtonWidget();
    gridLayout->addWidget(_buttonWidget);
    row++;

    setupFilterWidget();
    gridLayout->addWidget(_filterWidget, row, colL);
}

void TableEditWidget::setupVerticalUI()
{
    // create a single column grid layout and fill the cells
    int col = 0;
    int row = 0;
    auto gridLayout = new QGridLayout(this);

    gridLayout->addWidget(view, row, col);
    row++;

    gridLayout->addWidget(stackedWidget, row, col);
    row++;

    setupButtonWidget();
    gridLayout->addWidget(_buttonWidget);
    row++;

    setupFilterWidget();
    stackedWidget->addWidget(_filterWidget);
    stackedWidget->setCurrentWidget(_filterWidget);
    gridLayout->addWidget(stackedWidget);
}

void TableEditWidget::setupFilterWidget()
{
    // place the filter items in a grid layout so they occupy one cell in parent layout
    QWidget *widget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(widget);

    // one row, three columns
    layout->addWidget(new QLabel(tr("Filter"), this), 0, 0);
    layout->addWidget(filterLineEdit,				  0, 1);
    layout->addWidget(filterSelectionComboBox,		  0, 2);

    _filterWidget = widget;
}

void TableEditWidget::setupButtonWidget()
{
    auto buttonWidget = new QWidget(this);
    auto buttonGridLayout = new QGridLayout(buttonWidget);

    switch (_orientation) {
    case Horizontal:
        buttonGridLayout->addWidget(addNewEntryPushButton, 0, 0);
        buttonGridLayout->addWidget(deleteEntryPushButton, 1, 0);
        break;
    case Vertical:
        buttonGridLayout->addWidget(addNewEntryPushButton, 0, 0);
        buttonGridLayout->addWidget(deleteEntryPushButton, 0, 1);
    default:
        break;
    }

    buttonWidget->setLayout(buttonGridLayout);
    _buttonWidget = buttonWidget;
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
    auto editDialog = getEntryEditDialog(this);
    editDialog->exec();
}


void TableEditWidget::editEntryRequested(const QModelIndex &selectedIndex)
{
    int rowId = model->index(selectedIndex.row(), 0).data().toInt();
    _entryEditDialog->loadEntry(rowId);
    stackedWidget->setCurrentWidget(_entryEditDialog);

    switch (_orientation) {
    case Horizontal:
        stackedWidget->show();
        _entryEditDialog->exec();
        stackedWidget->hide();
        break;
    case Vertical:
        _entryEditDialog->exec();
        stackedWidget->setCurrentWidget(_filterWidget);
    default:
        break;
    }
}

void TableEditWidget::deleteEntryRequested()
{
    const QModelIndex selectedIndex = view->selectionModel()->currentIndex();
    if(!selectedIndex.isValid()) {
        WARN(tr("No entry selected."));
        return;
    }

    stackedWidget->hide();
    //clearStackedWidget();
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

void TableEditWidget::databaseContentChanged()
{
    model->select();
    view->resizeColumnsToContents();
}
