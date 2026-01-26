#ifndef FLIGHTLOGENTRYEDITDIALOG_H
#define FLIGHTLOGENTRYEDITDIALOG_H

#include "entryeditdialog.h"
#include "src/database/databasecache.h"
#include "src/database/flightdata.h"
#include "src/gui/comboboxes/dbselectioncombobox.h"
#include "src/gui/dialogues/airportentryeditdialog.h"
#include "src/gui/dialogues/pilotentryeditdialog.h"
#include "src/gui/dialogues/tailentryeditdialog.h"
#include "src/opl.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimeEdit>
#include <qtmetamacros.h>

using MapType = OPL::DatabaseCache::MapType;

class FlightLogEntryEditDialog : public EntryEditDialog {
  public:
    explicit FlightLogEntryEditDialog(QWidget *parent = nullptr);
    void loadEntry(int rowID);
    bool deleteEntry(int rowID);

  private:
    // dialog setup
    void init();
    void retranslateUi();
    void setupValidationAndCompletion();
    void setupSlots();
    void readSettings();

    // dialog flow
    bool addNewEntry(DbSelectionComboBox *box);
    bool addNewDatabaseElement(DbSelectionComboBox *box);

    // data collection and verification
    // create FlighDataBuilder
    bool runSanityChecks();

    // UI Elements
    QGridLayout *gridLayout;
    QLabel *dateDisplayLabel;
    QLabel *deptLabel;
    QLabel *deptDisplayLabel;
    QLabel *destLabel;
    QLabel *destDisplayLabel;
    QLabel *timeOffLabel;
    QLabel *timeOnLabel;
    QLabel *totalTimeLabel;
    QLabel *totalTimeDisplayLabel;
    QLabel *pilotFunctionLabel;
    QLabel *flightRulesLabel;
    QLabel *registrationLabel;
    QLabel *picLabel;
    QLabel *sicLabel;
    QLabel *flightNumberLabel;
    QLabel *takeOffCountLabel;
    QLabel *landingCountLabel;
    QLabel *remarksLabel;
    QLineEdit *deptLineEdit;
    QLineEdit *destLineEdit;
    QTimeEdit *timeOffEdit;
    QTimeEdit *timeOnEdit;
    DbSelectionComboBox *registrationComboBox;
    DbSelectionComboBox *picComboBox;
    DbSelectionComboBox *sicComboBox;
    QLineEdit *flightNumberLineEdit;
    QPushButton *datePushButton;
    QCheckBox *pilotFlyingCheckBox;
    QDateEdit *dateEdit;
    QComboBox *pilotFunctionComboBox;
    QComboBox *flightRulesComboBox;
    QSpinBox *takeOffCountSpinBox;
    QSpinBox *landingCountSpinBox;
    QPlainTextEdit *remarksTextEdit;

    QDialogButtonBox *buttonBox;

    QList<QLineEdit *> m_locationLineEdits;

    int m_eventId        = 0;
    bool m_addNewOffered = false; // de-bounce repeated triggering of editing finished by QCompleter
    bool m_addNewDialogExecuted = false;

    const QString m_dateFormatString;
    const QString m_timeFormatString;

  private slots:
    void on_accepted();
    void on_selectionComboBox_unkownValueEntered(DbSelectionComboBox *caller);
    void on_pilotFlyingCheckBoxStateChanged(Qt::CheckState state);
    void inline on_badInputReceived(QWidget *caller)
    {
        caller->setStyleSheet(OPL::CssStyles::RED_BORDER);
    }
    void inline on_GoodInputReceived(QWidget *caller) { caller->setStyleSheet(QString()); }
};

#endif // FLIGHTLOGENTRYEDITDIALOG_H
