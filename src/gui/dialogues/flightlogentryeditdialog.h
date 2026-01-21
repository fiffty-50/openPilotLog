#ifndef FLIGHTLOGENTRYEDITDIALOG_H
#define FLIGHTLOGENTRYEDITDIALOG_H

#include "entryeditdialog.h"
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

class FlightLogEntryEditDialog : public EntryEditDialog {
  public:
    explicit FlightLogEntryEditDialog(QWidget *parent = nullptr);
    void loadEntry(int rowID);
    bool deleteEntry(int rowID);

  private:
    void init();
    void retranslateUi();
    void setupValidationAndCompletion();
    void setupSlots();

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
    QPushButton *datePushButton;
    QCheckBox *pilotFlyingCheckBox;
    QDateEdit *dateEdit;
    QLineEdit *deptLineEdit;
    QLineEdit *destLineEdit;
    QTimeEdit *timeOffEdit;
    QTimeEdit *timeOnEdit;
    QComboBox *pilotFunctionComboBox;
    QComboBox *flightRulesComboBox;
    QLineEdit *registrationLineEdit;
    QLineEdit *picLineEdit;
    QLineEdit *sicLineEdit;
    QLineEdit *flightNumberLineEdit;
    QSpinBox *takeOffCountSpinBox;
    QSpinBox *landingCountSpinBox;
    QPlainTextEdit *remarksTextEdit;

    QDialogButtonBox *buttonBox;

    QList<QLineEdit*> m_nameLineEdits;
    QList<QLineEdit*> m_locationLineEdits;

    private slots:
        void on_accepted();
};

#endif // FLIGHTLOGENTRYEDITDIALOG_H
