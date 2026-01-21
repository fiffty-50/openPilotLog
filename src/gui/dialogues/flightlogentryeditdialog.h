#ifndef FLIGHTLOGENTRYEDITDIALOG_H
#define FLIGHTLOGENTRYEDITDIALOG_H

#include "entryeditdialog.h"
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
    QLineEdit *deptLineEdit;
    QLineEdit *destLineEdit;
    QTimeEdit *timeOffEdit;
    QTimeEdit *timeOnEdit;
    QLineEdit *registrationLineEdit;
    QLineEdit *picLineEdit;
    QLineEdit *sicLineEdit;
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

    QList<QLineEdit *> m_nameLineEdits;
    QList<QLineEdit *> m_locationLineEdits;

  private slots:
    void on_accepted();

    void on_locationLineEdit_editingFinished(QLineEdit *caller, QLabel *displayLabel);
    void on_registrationLineEdit_editingFinished();
    void on_pilotNameLineEdit_editingFinished(QLineEdit *caller);
    void on_pilotFlyingCheckBoxStateChanged(Qt::CheckState state);
    void inline on_badInputReceived(QWidget *caller)
    {
        caller->setStyleSheet(OPL::CssStyles::RED_BORDER);
    }
    void inline on_GoodInputReceived(QWidget *caller) { caller->setStyleSheet(QString()); }
};

#endif // FLIGHTLOGENTRYEDITDIALOG_H
