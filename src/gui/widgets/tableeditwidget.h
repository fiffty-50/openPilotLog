#ifndef TABLEEDITWIDGET_H
#define TABLEEDITWIDGET_H

#include "src/gui/dialogues/entryeditdialog.h"
#include <QComboBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlTableModel>
#include <QStackedWidget>
#include <QTableView>
#include <QWidget>

/*!
 * \brief The TableEditWidget class is a base class for widgets which enable
 * editing certain tables in the datbase.
 * \details The TableEditWidget consists of a QTableView which displays the data
 * from a given table. The data is held in a QSqlTableModel. The views edit triggers
 * are disabled. Whenever a row is selected in the view, the selected entry is displayed
 * for editing in a suitable EntryEditDialog which is responsible for the verification of
 * the user input as well as reading and writing to and from the database.
 *
 *
 * The table view is split horizontally to make space for the TableEditWidget on the right hand
 * side. The Dialog is shown in a non-modal way for editing and as a modal dialog when adding
 * a new entry.
 *
 * When implementing the TableEditWidget it is important to set up the model and call the
 * Base Class implementation of setupUI before performing any specialisations. Before the
 * TableEditWidget is shown, the init method must be run.
 */
class TableEditWidget : public QWidget {
    Q_OBJECT
  public:
    /*!
     * \brief Create a new TableEditWidget
     */
    explicit TableEditWidget(QWidget *parent = nullptr);

    /*!
     * \brief Initialises the dialog by calling its virtual setup functions.
     * \attention Call this functian before showing the dialog.
     */
    void init();

    /*!
     * \brief Set up the model and view of the widget
     * \details Implement this function to initialise the protected members of this class.
     * This includes setting the QSqlTableModel and QTableView      */
    virtual void setupModelAndView();

    /*!
     * \brief Set up the UI of the widget
     * \details Implement this function to set appropriate labels to the protected members of this
     * class. This includes setting appropriate labels on the Push Buttons as well as
     * appropriate filter options in the filter Combo Box. Make sure to call the base class
     * implementation first when overriding this method.
     */
    virtual void setupUI();

    /*!
     * \brief Set the user facing Strings in the UI
     */
    virtual void retranslateUi() = 0;

    /*!
     * \brief create an error String when deleting a database entry has been unsuccessful
     * \param rowId - the row id of the entry to be deleted
     * \details When deleting an entry from a database fails, this can have different reasons
     * depending on the table. This function returns an implementation-specific error string
     * to inform the user about the failure and give hints on how to fix it
     */
    virtual QString deleteErrorString(int rowId) = 0;

    /*!
     * \brief return a String asking the user to confirm deletion of a given entry
     * \param rowId - the row id of the entry to be deleted
     * \brief The message string is displayed in a QMessageBox
     */
    virtual QString confirmDeleteString(int rowId) = 0;

    /*!
     * \brief get an apropriate Edit Dialog for the implementation
     * \details The Edit Dialogs for different tables differ in the data they display
     * and how they verify the user inputs. This method returns an apropriate
     * EntryEditDialog for the selected table.
     * \return An instance of EntryEditDialog. The caller owns the returned dialog.
     */
    virtual EntryEditDialog *createEntryEditDialog() = 0;

  protected:
    EntryEditDialog *m_entryEditDialog = nullptr;

    QStackedWidget *m_stackedWidget      = new QStackedWidget(this);
    QTableView *m_view                   = new QTableView(this);
    QPushButton *m_addNewEntryPushButton = new QPushButton(this);
    QPushButton *m_deleteEntryPushButton = new QPushButton(this);
    QLineEdit *m_filterLineEdit          = new QLineEdit(this);
    QComboBox *m_filterSelectionComboBox = new QComboBox(this);
    QSqlTableModel *m_model              = nullptr;
    QWidget *m_filterWidget              = nullptr;
    QWidget *m_buttonWidget              = nullptr;

    /*!
     * \brief Return an array of column numbers of the model that are visible in the UI
     */
    virtual const QList<int> *getVisibleColumns() const = 0;

    /*!
     * \brief Return an array of column numbers of the model that are hidden in the UI
     */
    virtual const QList<int> *getHiddenColumns() const = 0;

    /*!
     * \brief Return a map of <Column Number, Column Header> of the model
     */
    virtual const QMap<int, QString> *getColumnHeaderMap() const = 0;

    virtual const QString tableName() const = 0;

    virtual void showEditWidget();
    virtual void hideEditWidget();

    /*!
     * \brief return a SQL filter statement for a given column name and filter text
     */
    QString getFilterStatement(const QString &column, const QString &filterText);

  private:
    void setupSignalsAndSlots();

    /*!
     * \brief Place the filter items in a widget to facilitate easier placement in parent layout
     */
    void setupFilterWidget();

    /*!
     * \brief Place the new and edit buttons in a widget to facilitate easier placement in parent
     * layout
     */
    void setupButtonWidget();

  public slots:
    virtual void openEntryEdit(std::optional<int> rowId);
    virtual void deleteEntryRequested();
    virtual void sortColumnChanged(int newSortColumn);

    /*!
     * \brief Set a filter on the model
     */
    virtual void filterTextChanged(const QString &filterString);

  public slots:
    /*!
     * \brief refresh the view after a Database change
     */
    void databaseContentChanged();
};

#endif // TABLEEDITWIDGET_H
