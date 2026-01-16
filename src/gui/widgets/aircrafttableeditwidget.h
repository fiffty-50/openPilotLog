#ifndef AIRCRAFTTABLEEDITWIDGET_H
#define AIRCRAFTTABLEEDITWIDGET_H

#include "tableeditwidget.h"

class AircraftTableEditWidget : public TableEditWidget
{
    Q_OBJECT
public:
    AircraftTableEditWidget() = delete;
    explicit AircraftTableEditWidget(QWidget *parent = nullptr);

    // TableEditWidget interface
    void setupModelAndView() override;
    void setupUI() override;
    QString deleteErrorString(int rowId) override;
    QString confirmDeleteString(int rowId) override;
    EntryEditDialog *getEntryEditDialog(QWidget *parent) override;

private:
    void retranslateUi();
    QStringList FILTER_COLUMNS = {};
    static constexpr int COL_ROWID = 0;
    static constexpr int COL_MAKE = 1;
    static constexpr int COL_MODEL = 2;
    static constexpr int COL_VARIANT = 3;
    static constexpr int COL_ICAO_DESIGNATOR = 4;
    static constexpr int COL_ENGINE_TYPE = 5;
    static constexpr int COL_ENGINE_COUNT = 6;
    static constexpr int COL_CLASS = 7;
    static constexpr int COL_SUB_CLASS = 8;
    static constexpr int COL_TYPE_RATING = 9;
    static constexpr int COL_REMARKS = 10;

    const QHash<int, QString> COLUMN_HEADERS_MAP = {
        {COL_MAKE, 				tr("Make")},
        {COL_MODEL, 			tr("Model")},
        {COL_VARIANT, 			tr("Variant")},
        {COL_ICAO_DESIGNATOR, 	tr("ICAO Designator")},
        {COL_ENGINE_TYPE, 		tr("Engine Type")},
        {COL_ENGINE_COUNT, 		tr("Engine Count")},
        {COL_CLASS, 			tr("Class")},
        {COL_SUB_CLASS, 		tr("Sub Class")},
        {COL_TYPE_RATING, 		tr("Type Rating")},
        {COL_REMARKS, 			tr("Remarks")},
    };

private slots:
    // TableEditWidget interface
    void filterTextChanged(const QString &filterString) override;
};

#endif // AIRCRAFTTABLEEDITWIDGET_H
