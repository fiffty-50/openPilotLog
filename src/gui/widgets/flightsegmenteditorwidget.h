#ifndef FLIGHTSEGMENTEDITORWIDGET_H
#define FLIGHTSEGMENTEDITORWIDGET_H

#include "src/database/entries/flightsegmententry.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QTimeEdit>
#include <QVector>
#include <QWidget>

class FlightSegmentEditorWidget : public QWidget {
    Q_OBJECT

  public:
    explicit FlightSegmentEditorWidget(QWidget *parent = nullptr);

    void loadSegments(const QList<OPL::FlightSegmentEntry> &segments);

  private:
    struct SegmentRow {
        QTimeEdit *startEdit;
        QTimeEdit *endEdit;
        QCheckBox *isIfrCheckBox;
        QCheckBox *isSimIfrCheckBox;
        QCheckBox *isNightCheckBox;
        QCheckBox *isMultiPilotCheckBox;
        QCheckBox *isPilotFlyingCheckBox;
        QComboBox *pilotFunctionComboBox;
    };

    void addRow();

  private:
    QGridLayout *gridLayout;
    QPushButton *addSegmentButton;
    QDialogButtonBox *buttonBox;
    QLabel *startLabel;
    QLabel *endLabel;
    QLabel *isIfrLabel;
    QLabel *isSimIfrLabel;
    QLabel *isNightLabel;
    QLabel *isMultiPilotLabel;
    QLabel *isPilotFlyingLabel;
    QLabel *pilotFunctionLabel;

    QVector<SegmentRow> m_rows;
    int m_currentRow;
    static constexpr int m_r1           = 0;
    static constexpr int m_r2           = 1;
    static constexpr int m_r3           = 2;
    static constexpr int m_r4           = 3;
    static constexpr int m_r5           = 4;
    static constexpr int m_r6           = 5;
    static constexpr int m_r7           = 6;
    static constexpr int m_r8           = 7;
    static constexpr int m_r9           = 8;
    static constexpr int SINGLE_SPAN    = 1;
    static constexpr int SPAN_REMAINING = -1;
    const QString m_timeFormatString;
};

#endif // FLIGHTSEGMENTEDITORWIDGET_H
