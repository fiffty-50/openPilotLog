/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "tableeditwidget.h"
#include "src/database/database.h"
#include "src/opl.h"
#include <QGridLayout>
#include <QLabel>

TableEditWidget::TableEditWidget(Orientation orientation, QWidget *parent)
    : QWidget{parent}, m_orientation(orientation)
{
}

void TableEditWidget::init()
{
    setupUI();
    setupSignalsAndSlots();
}

void TableEditWidget::setupUI()
{
    setupModelAndView();
    m_entryEditDialog = createEntryEditDialog();
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
    retranslateUi();
}

void TableEditWidget::setupHorizontalUI()
{
    // In the horizontal view, the editing widget is hidden on the right hand side
    m_stackedWidget->hide();

    // create a 2-column grid layout and fill the cells
    constexpr int colL       = 0; // left column
    constexpr int colR       = 1; // right column
    constexpr int allSpan    = -1;
    constexpr int singleSpan = 1;
    int row                  = 0;

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
    int row           = 0;
    auto gridLayout   = new QGridLayout(this);

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
    QWidget *widget     = new QWidget(this);
    QGridLayout *layout = new QGridLayout(widget);

    // one row, three columns
    layout->addWidget(new QLabel(tr("Search"), this), 0, 0);
    layout->addWidget(m_filterLineEdit, 0, 1);
    layout->addWidget(m_filterSelectionComboBox, 0, 2);

    m_filterWidget = widget;

    // Set Up the Filtering Box
    m_filterSelectionComboBox->clear();
    m_filterSelectionComboBox->addItem("All Columns", 0);
    for (auto it = getColumnHeaderMap()->cbegin(); it != getColumnHeaderMap()->cend(); ++it) {
        m_filterSelectionComboBox->addItem(it.value(), it.key());
    }
}

void TableEditWidget::setupButtonWidget()
{
    auto buttonWidget     = new QWidget(this);
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

void TableEditWidget::setupModelAndView()
{
    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(tableName());
    m_model->select();

    for (auto it = getColumnHeaderMap()->cbegin(); it != getColumnHeaderMap()->cend(); ++it) {
        m_model->setHeaderData(it.key(), Qt::Horizontal, it.value());
    }

    for (auto it = getHiddenColumns()->cbegin(); it != getHiddenColumns()->cend(); ++it) {
        m_view->hideColumn(*it);
    }

    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    m_view->resizeColumnsToContents();
    m_view->verticalHeader()->hide();
    m_view->setAlternatingRowColors(true);
    m_view->hideColumn(0); // hide Row ID
}

void TableEditWidget::setupSignalsAndSlots()
{
    // refresh the view when the database is updated
    QObject::connect(DB, &OPL::Database::dataBaseUpdated, this,
                     &TableEditWidget::databaseContentChanged);

    // filter the view
    QObject::connect(m_filterLineEdit, &QLineEdit::textChanged, this,
                     &TableEditWidget::filterTextChanged);

    // update filter when combo box is changed
    QObject::connect(m_filterSelectionComboBox, &QComboBox::currentIndexChanged, this,
                     [this]() { filterTextChanged(m_filterLineEdit->text()); });

    // sort the view by column
    QObject::connect(m_view->horizontalHeader(), &QHeaderView::sectionClicked, this,
                     &TableEditWidget::sortColumnChanged);

    // Force the view to update the selected row when a column is selected
    connect(m_view->selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this](const QModelIndex &current, const QModelIndex &) {
                if (current.isValid()) m_view->selectRow(current.row());
            });

    // Edit an entry when selected with arrow keys
    QObject::connect(m_view->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
                     [this](const QModelIndex &current, const QModelIndex &previous) {
                         Q_UNUSED(previous);
                         const int rowId = current.sibling(current.row(), 0).data().toInt();
                         DEB << "entry edit blocked.";
                         //openEntryEdit(rowId);
                     });

    // Add a new Entry
    QObject::connect(m_addNewEntryPushButton, &QPushButton::clicked, this,
                     [this]() { openEntryEdit(std::nullopt); });

    // Delete a selected entry
    QObject::connect(m_deleteEntryPushButton, &QPushButton::clicked, this,
                     &TableEditWidget::deleteEntryRequested);
}

void TableEditWidget::openEntryEdit(std::optional<int> rowId)
{
    cleanUpOldEditDialog();

    m_entryEditDialog = createEntryEditDialog();

    if (rowId) m_entryEditDialog->loadEntry(*rowId);

    m_stackedWidget->addWidget(m_entryEditDialog);
    m_stackedWidget->setCurrentWidget(m_entryEditDialog);

    showEditWidget();
    m_entryEditDialog->exec();
    hideEditWidget();
}

void TableEditWidget::deleteEntryRequested()
{
    // get the row Id
    const QModelIndex index = m_view->selectionModel()->currentIndex();

    if (!index.isValid()) {
        WARN(tr("No entry selected."));
        return;
    }

    const int rowId = m_model->index(index.row(), 0).data().toInt();

    m_view->selectionModel()->reset();
    m_stackedWidget->hide();

    // get user confirmation
    const auto answer =
        QMessageBox::question(this, tr("Confirm Deletion"), confirmDeleteString(rowId),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (answer == QMessageBox::Yes) {
        auto dialog = createEntryEditDialog();
        if (!dialog->deleteEntry(rowId)) WARN(deleteErrorString(rowId));
    }

    // re-set for vertical layout
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
    return QString(QLatin1Char('\"') + column + QLatin1String("\" LIKE '%") + filterText +
                   QLatin1String("%'"));
}

void TableEditWidget::cleanUpOldEditDialog()
{
    if (!m_entryEditDialog) return;

    m_stackedWidget->removeWidget(m_entryEditDialog);
    m_entryEditDialog->deleteLater();
    m_entryEditDialog = nullptr;
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
