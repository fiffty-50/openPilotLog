#ifndef AIRPORTENTRYEDITDIALOG_H
#define AIRPORTENTRYEDITDIALOG_H

#include "src/gui/dialogues/entryeditdialog.h"
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>

class AirportEntryEditDialog : public EntryEditDialog
{
    Q_OBJECT

public:
    explicit AirportEntryEditDialog(QWidget *parent = nullptr);
    explicit AirportEntryEditDialog(int row_id, QWidget* parent = nullptr);
    ~AirportEntryEditDialog() = default;



private slots:
    void on_buttonBox_accepted();

private:
    // UI Elements
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QLabel *countryLabel;
    QLabel *longitudeLabel;
    QLabel *latitudeLabel;
    QLabel *timezoneLabel;
    QLineEdit *countryLineEdit;
    QComboBox *timeZoneComboBox;
    QLabel *icaoLabel;
    QLabel *icaoDisplayLabel;
    QLabel *iataLabel;
    QLabel *iataDisplayLabel;
    QLineEdit *nameLineEdit;
    QLabel *nameLabel;
    QDoubleSpinBox *lonDoubleSpinBox;
    QDoubleSpinBox *latDoubleSpinBox;

    // Member Variables
    int m_rowId;

    // Member Functions
    void init();
    void retranslateUi();
    void setupSlots();
    void loadTimeZones();
    bool confirmTimezone();
    void loadAirportData(int row_id);
    bool verifyInput();


    // EntryEditDialog interface
public:
    virtual void loadEntry(int rowId) override;
    virtual bool deleteEntry(int rowId) override;
    virtual void loadEntry(const OPL::Row &entry) override;
};

#endif // AIRPORTENTRYEDITDIALOG_H
