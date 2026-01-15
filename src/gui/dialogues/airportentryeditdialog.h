#ifndef AIRPORTENTRYEDITDIALOG_H
#define AIRPORTENTRYEDITDIALOG_H

#include "src/gui/dialogues/entryeditdialog.h"
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class AirportEntryEditDialog : public EntryEditDialog
{
    Q_OBJECT

public:
    explicit AirportEntryEditDialog(QWidget *parent = nullptr);
    explicit AirportEntryEditDialog(int row_id, QWidget* parent = nullptr);
    ~AirportEntryEditDialog() = default;



private slots:
    void on_editIcaoCodePushButton_clicked();
    void on_editIataCodePushButton_clicked();
    void on_editOtherCodePushButton_clicked();
    void on_buttonBox_accepted();

private:
    // UI Elements
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QLineEdit *nameLineEdit;
    QLabel *nameLabel;
    QLabel *longitudeLabel;
    QLabel *latitudeLabel;
    QLabel *timezoneLabel;
    QLabel *icaoLabel;
    QLabel *icaoDisplayLabel;
    QLabel *iataLabel;
    QLabel *iataDisplayLabel;
    QLabel *otherCodeLabel;
    QLabel *otherCodeDisplayLabel;
    QPushButton *editIcaoCodePushButton;
    QPushButton *editIataCodePushButton;
    QPushButton *editOtherCodePushButton;
    QDoubleSpinBox *lonDoubleSpinBox;
    QDoubleSpinBox *latDoubleSpinBox;
    QComboBox *timeZoneComboBox;

    // Member Variables
    int m_rowId;

    // Member Functions
    void init();
    void retranslateUi();
    void setupSlots();
    void loadTimeZones();
    bool confirmTimezone();
    void loadAirportData(int row_id);
    bool userWantsToEditCode();


    // EntryEditDialog interface
public:
    virtual void loadEntry(int rowId) override;
    virtual bool deleteEntry(int rowId) override;
    virtual void loadEntry(const OPL::Row &entry) override;
};

#endif // AIRPORTENTRYEDITDIALOG_H
