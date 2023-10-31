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
 */
class TableEditWidget : public QWidget
{
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
    QSqlTableModel *model = nullptr;
    QTableView *view = new QTableView(this);

    QPushButton *addNewEntryPushButton = new QPushButton(this);
    QPushButton *deleteEntryPushButton = new QPushButton(this);

    QStackedWidget *stackedWidget = new QStackedWidget(this);
    QLineEdit *filterLineEdit = new QLineEdit(this);
    QComboBox *filterSelectionComboBox = new QComboBox(this);

private:
    void setupSignalsAndSlots();
    QWidget *setupFilterWidget();

    /*!
     * \brief Remove orphaned editing widgets from the stacked widget.
     * \details When the user selects another entry without finishing editing a previous one,
     * the previously created entry edit never finished. To prevent a leak, this function is called
     * to clean up stale dialogs
     */
    void clearStackedWidget();

private slots:
    void addEntryRequested();
    void editEntryRequested(const QModelIndex &selectedIndex);
    void deleteEntryRequested();
    void filterTextChanged(const QString &filterString);
    void sortColumnChanged(int newSortColumn);

public slots:
    void databaseContentChanged();

};

#endif // TABLEEDITWIDGET_H
