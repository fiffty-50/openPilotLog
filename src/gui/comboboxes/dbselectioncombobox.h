#ifndef DBSELECTIONCOMBOBOX_H
#define DBSELECTIONCOMBOBOX_H

#include <QComboBox>
#include <QCompleter>
#include <QLineEdit>
#include <QObject>
#include <QSqlRecord>
#include <QStringListModel>

/*!
 * \brief The DbSelectionComboBox class extends QComboBox with logic for selecting aircraft
 * registrations from the database.
 * \details This combo box uses a QSqlQueryModel <pilot_name, pilot_id> where the data value of each
 * user-facing string contains the row_id of the associated database entry. It also includes a
 * DiacriticIgnoringCompleter to enable searching the available registrations with or without a '-'
 * character.
 *
 * When an entry is made that is not contained in the database, newValueEntered is emitted.
 *
 * The DbSelectionComboBox is connected to the OPL::Database::databaseUpdated signal and refreshes
 * itself when a database change is signalled.
 */
class DbSelectionComboBox : public QComboBox {
    Q_OBJECT
  public:
    enum CompletionTarget { TailRegistrations, PilotNames, AirportCodes };
    Q_ENUM(CompletionTarget);

    DbSelectionComboBox(CompletionTarget target, QWidget *parent = nullptr);
    CompletionTarget getCompletionTarget() const { return m_completionTarget; }

  signals:
    /*!
     *\brief When the user enters a registration not contained in the model newValueEntered is
     * emitted.
     */
    void newValueEntered(DbSelectionComboBox *caller);

  private slots:
    void on_editingFinished();

  private:
    void connectSlots();
    void refresh();
    bool completionIsAvailable();

    // value -> row_id
    QHash<QString, int> m_map;
    CompletionTarget m_completionTarget;

    const static QString getQuery(CompletionTarget target)
    {
        switch (target) {
        case DbSelectionComboBox::TailRegistrations:
            return QStringLiteral("SELECT tail_id, registration FROM aircraft_tails");
        case DbSelectionComboBox::PilotNames:
            return QStringLiteral("SELECT pilot_id, pilot_name FROM pilots");
        case DbSelectionComboBox::AirportCodes:
            return QStringLiteral("WITH CurrentCode AS ( "
                                  "SELECT "
                                  "airport_id, "
                                  "airport_code, "
                                  "valid_from_jd, "
                                  "valid_to_jd "
                                  "FROM airport_codes "
                                  "WHERE (valid_to_jd IS NULL OR valid_to_jd >= julianday('now')) "
                                  "AND valid_from_jd <= julianday('now')) "
                                  "SELECT "
                                  "airport_id, "
                                  "airport_code "
                                  "FROM CurrentCode ");
            break;
        default:
            Q_UNREACHABLE();
        }
    }
};

#endif // DBSELECTIONCOMBOBOX_H
