#ifndef TABLEEDITWIDGET_H
#define TABLEEDITWIDGET_H

#include "src/gui/dialogues/entryeditdialog.h"
#include <QWidget>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QComboBox>
#include <QLabel>

/*!
 * \brief The TableEditWidget class is a base class for widgets which enable
 * editing certain tables in the datbase.
 * \details The TableEditWidget consists of a QTableView which displays the data
 * from a given table. The data is held in a QSqlTableModel. The views edit triggers
 * are disabled. Whenever a row is selected in the view, the selected entry is displayed
 * for editing in a suitable EntryEditDialog which is responsible for the verification of
 * the user input as well as reading and writing to and from the database.
 *
 * The TableEditWidget has two Orientation options: Horizontal and Vertical
 *
 * In the Horizontal layout, the table view is split horizontally to make space
 * for the TableEditWidget on the right hand side, whereas on the Vertical layout
 * it is split vertically with the TableEditWidget occupying the lower half of the screen.
 *
 * When implementing the TableEditWidget it is important to set up the model and call the
 * Base Class implementation of setupUI before performing any specialisations. Before the
 * TableEditWidget is shown, the init method must be run.
 */
class TableEditWidget : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Determines how the layout is created
     * \details <ul>
     * <li> Horizontal: The edit widget is shown besides the table </li>
     * <li> Vertical: The edit widget is shown below the table </li>
     * </ul>
     */
    enum Orientation {Horizontal, Vertical};

    /*!
     * \brief Create a new TableEditWidget
     */
    explicit TableEditWidget(Orientation orientation = Horizontal, QWidget *parent = nullptr);

    /*!
     * \brief Initialises the dialog by calling its virtual setup functions.
     * \attention Call this functian before showing the dialog.
     */
    void init();

    /*!
     * \brief Set up the model and view of the widget
     * \details Implement this function to initialise the protected members of this class.
     * This includes setting the QSqlTableModel and QTableView      */
    virtual void setupModelAndView() = 0;

    /*!
     * \brief Set up the UI of the widget
     * \details Implement this function to set appropriate labels to the protected members of this
     * class. This includes setting appropriate labels on the Push Buttons as well as
     * appropriate filter options in the filter Combo Box. Make sure to call the base class
     * implementation first when overriding this method.
     */
    virtual void setupUI();

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
     */
    virtual EntryEditDialog *getEntryEditDialog(QWidget *parent = nullptr) = 0;

protected:
    Orientation m_orientation;
    QSqlTableModel *m_model = nullptr;
    QTableView *m_view = new QTableView(this);
    QWidget *m_filterWidget = nullptr;
    QWidget *m_buttonWidget = nullptr;
    EntryEditDialog *m_entryEditDialog = nullptr;

    QPushButton *m_addNewEntryPushButton = new QPushButton(this);
    QPushButton *m_deleteEntryPushButton = new QPushButton(this);

    QStackedWidget *m_stackedWidget = new QStackedWidget(this);
    QLineEdit *m_filterLineEdit = new QLineEdit(this);
    QComboBox *m_filterSelectionComboBox = new QComboBox(this);

private:
    void setupHorizontalUI();
    void setupVerticalUI();
    void setupSignalsAndSlots();

    /*!
     * \brief Place the filter items in a widget to facilitate easier placement in parent layout
     */
    void setupFilterWidget();

    /*!
     * \brief Place the new and edit buttons in a widget to facilitate easier placement in parent layout
     */
    void setupButtonWidget();

private slots:
    void addEntryRequested();
    void editEntryRequested(const QModelIndex &selectedIndex);
    void deleteEntryRequested();
    void sortColumnChanged(int newSortColumn);

    /*!
     * \brief Set a filter on the model
     */
    virtual void filterTextChanged(const QString &filterString) = 0;

public slots:
    void databaseContentChanged();

};

#endif // TABLEEDITWIDGET_H
