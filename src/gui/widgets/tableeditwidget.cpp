#include "tableeditwidget.h"
#include "src/database/database.h"
#include "src/opl.h"
#include <QGridLayout>
#include <QLabel>

TableEditWidget::TableEditWidget(Orientation orientation, QWidget *parent)
    : QWidget{parent}, m_orientation(orientation)
{}

void TableEditWidget::init()
{
    setupUI();
    setupSignalsAndSlots();
}

void TableEditWidget::setupUI()
{
    // Setting up the model and view is done in the derived class
    setupModelAndView();
    m_entryEditDialog = getEntryEditDialog(this);
    m_stackedWidget->addWidget(m_entryEditDialog);

    // set up the UI
    switch (m_orientation) {
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
    m_stackedWidget->hide();

    // create a 2-column grid layout and fill the cells
    constexpr int colL = 0; // left column
    constexpr int colR = 1; // right column
    constexpr int allSpan = -1;
    constexpr int singleSpan = 1;
    int row = 0;

    auto gridLayout = new QGridLayout(this);

    gridLayout->addWidget(m_view, row, colL);
    row++;

    setupButtonWidget();
    gridLayout->addWidget(m_buttonWidget);
    row++;

    setupFilterWidget();
    gridLayout->addWidget(m_filterWidget, row, colL);

    // add here so that it spans all rows properly
    gridLayout->addWidget(m_stackedWidget, 0, colR, allSpan, singleSpan);
}

void TableEditWidget::setupVerticalUI()
{
    // create a single column grid layout and fill the cells
    constexpr int col = 0;
    int row = 0;
    auto gridLayout = new QGridLayout(this);

    gridLayout->addWidget(m_view, row, col);
    row++;

    gridLayout->addWidget(m_stackedWidget, row, col);
    row++;

    setupButtonWidget();
    gridLayout->addWidget(m_buttonWidget);
    row++;

    setupFilterWidget();
    m_stackedWidget->addWidget(m_filterWidget);
    m_stackedWidget->setCurrentWidget(m_filterWidget);
    gridLayout->addWidget(m_stackedWidget);
}

void TableEditWidget::setupFilterWidget()
{
    // place the filter items in a grid layout so they occupy one cell in parent layout
    QWidget *widget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(widget);

    // one row, three columns
    layout->addWidget(new QLabel(tr("Search"), this), 0, 0);
    layout->addWidget(m_filterLineEdit, 0, 1);
    layout->addWidget(m_filterSelectionComboBox, 0, 2);

    m_filterWidget = widget;

    // Set Up the Filtering Box
    m_filterSelectionComboBox->clear();
    m_filterSelectionComboBox->addItem("All Columns", 0);
    for(auto it = getColumnHeaderMap()->cbegin(); it != getColumnHeaderMap()->cend(); ++it) {
        m_filterSelectionComboBox->addItem(it.value(), it.key());
    }
}

void TableEditWidget::setupButtonWidget()
{
    auto buttonWidget = new QWidget(this);
    auto buttonGridLayout = new QGridLayout(buttonWidget);

    switch (m_orientation) {
    case Horizontal:
        buttonGridLayout->addWidget(m_addNewEntryPushButton, 0, 0);
        buttonGridLayout->addWidget(m_deleteEntryPushButton, 1, 0);
        break;
    case Vertical:
        buttonGridLayout->addWidget(m_addNewEntryPushButton, 0, 0);
        buttonGridLayout->addWidget(m_deleteEntryPushButton, 0, 1);
    default:
        break;
    }

    m_buttonWidget = buttonWidget;
}

void TableEditWidget::setupSignalsAndSlots()
{
    // refresh the view when the database is updated
    QObject::connect(DB, &OPL::Database::dataBaseUpdated,
                     this, &TableEditWidget::databaseContentChanged);
    // filter the view
    QObject::connect(m_filterLineEdit, &QLineEdit::textChanged,
                     this, &TableEditWidget::filterTextChanged);
    // update filter when combo box is changed
    QObject::connect(m_filterSelectionComboBox, &QComboBox::currentIndexChanged,
                     this, [this]()
                     { filterTextChanged(m_filterLineEdit->text()); });
    // sort the view by column
    QObject::connect(m_view->horizontalHeader(), &QHeaderView::sectionClicked,
                     this, &TableEditWidget::sortColumnChanged);
    // Edit an entry
    QObject::connect(m_view, &QTableView::clicked,
                     this, &TableEditWidget::editEntryRequested);
    // Add a new entry
    QObject::connect(m_addNewEntryPushButton, &QPushButton::clicked,
                     this, &TableEditWidget::addEntryRequested);
    // Delete a selected entry
    QObject::connect(m_deleteEntryPushButton, &QPushButton::clicked,
                     this, &TableEditWidget::deleteEntryRequested);
}

void TableEditWidget::addEntryRequested()
{
    cleanUpOldEditDialog();

    m_entryEditDialog = getEntryEditDialog(this);
    m_stackedWidget->addWidget(m_entryEditDialog);
    m_stackedWidget->setCurrentWidget(m_entryEditDialog);

    showEditWidget();
    m_entryEditDialog->exec();
    hideEditWidget();
}

void TableEditWidget::editEntryRequested(const QModelIndex &selectedIndex)
{
    int rowIdToEdit = m_model->index(selectedIndex.row(), 0).data().toInt();

    cleanUpOldEditDialog();

    m_entryEditDialog = getEntryEditDialog(this);
    m_entryEditDialog->loadEntry(rowIdToEdit);
    m_stackedWidget->addWidget(m_entryEditDialog);
    m_stackedWidget->setCurrentWidget(m_entryEditDialog);

    showEditWidget();
    m_entryEditDialog->exec();
    hideEditWidget();
}

void TableEditWidget::deleteEntryRequested()
{
    const QModelIndex selectedIndex = m_view->selectionModel()->currentIndex();
    if (!selectedIndex.isValid()) {
        WARN(tr("No entry selected."));
        return;
    }
    m_stackedWidget->hide();

    int rowId = m_model->index(selectedIndex.row(), 0).data().toInt();
    m_view->selectionModel()->reset();

    // get user confirmation
    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(tr("Confirm Deletion"));

    confirm.setText(confirmDeleteString(rowId));
    if (confirm.exec() == QMessageBox::Yes) {
        auto editDialog = getEntryEditDialog(this);
        if (!editDialog->deleteEntry(rowId))
            WARN(deleteErrorString(rowId));
    }

    // re-set stackedWidget for Vertical Layout
    if (m_orientation == Vertical) {
        m_stackedWidget->setCurrentWidget(m_filterWidget);
        m_stackedWidget->show();
    }
}

void TableEditWidget::sortColumnChanged(int newSortColumn)
{
    m_view->sortByColumn(newSortColumn, Qt::AscendingOrder);
}

void TableEditWidget::databaseContentChanged()
{
    m_model->select();
    m_view->resizeColumnsToContents();
}

void TableEditWidget::showEditWidget()
{
    // m_buttonWidget->hide();
    m_stackedWidget->show();
}

void TableEditWidget::hideEditWidget()
{
    m_stackedWidget->hide();
    // m_buttonWidget->show();
}

QString TableEditWidget::getFilterStatement(const QString &column, const QString &filterText)
{
    return QString(
        QLatin1Char('\"') + column + QLatin1String("\" LIKE '%") + filterText + QLatin1String("%'"));
}

void TableEditWidget::cleanUpOldEditDialog()
{
    if (m_stackedWidget->indexOf(m_entryEditDialog) != -1) {
        delete m_entryEditDialog;
    }
}

void TableEditWidget::filterTextChanged(const QString &filterText) 
{
        // Retreive selected column for filtering
    int column = m_filterSelectionComboBox->currentData().toInt();

    if (column == 0) {
        // search in all columns
        QString filter;
        const QString SQL_OR = QStringLiteral(" OR ");
        for (const auto &col : *getVisibleColumns()) {
            filter.append(getFilterStatement(m_model->record().fieldName(col), filterText));
            filter.append(SQL_OR);
        }
        // remove last "or"
        filter.chop(SQL_OR.size());
        m_model->setFilter(filter);
    }
    else {
        // filter based on selected column
        const QString filterColumn = m_model->record().fieldName(column);
        m_model->setFilter(getFilterStatement(filterColumn, filterText));
    }
}
