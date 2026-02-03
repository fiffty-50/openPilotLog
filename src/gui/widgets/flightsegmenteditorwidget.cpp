#include "flightsegmenteditorwidget.h"
#include "src/classes/settings.h"

FlightSegmentEditorWidget::FlightSegmentEditorWidget(QWidget *parent)
    : QWidget(parent), gridLayout(new QGridLayout(this)),
      addSegmentButton(new QPushButton(tr("Add Segment"), this)),
      buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this)),
      startLabel(new QLabel(tr("Start"), this)), endLabel(new QLabel(tr("End"), this)),
      isIfrLabel(new QLabel(tr("IFR"), this)), isSimIfrLabel(new QLabel(tr("Sim IFR"), this)),
      isNightLabel(new QLabel(tr("Night"), this)),
      isMultiPilotLabel(new QLabel(tr("Multi Pilot"), this)),
      isPilotFlyingLabel(new QLabel(tr("Pilot Flying"), this)),
      pilotFunctionLabel(new QLabel(tr("Function"), this)), m_currentRow(0),
      m_timeFormatString(Settings::getTimeFormatString())
{
    setLayout(gridLayout);

    // Create the header row
    // gridLayout->addWidget(startLabel, m_currentRow, m_r1, SINGLE_SPAN, SINGLE_SPAN);
    // gridLayout->addWidget(endLabel, m_currentRow, m_r2, SINGLE_SPAN, SINGLE_SPAN);
    // gridLayout->addWidget(isIfrLabel, m_currentRow, m_r3, SINGLE_SPAN, SINGLE_SPAN);
    // gridLayout->addWidget(isSimIfrLabel, m_currentRow, m_r4, SINGLE_SPAN, SINGLE_SPAN);
    // gridLayout->addWidget(isNightLabel, m_currentRow, m_r5, SINGLE_SPAN, SINGLE_SPAN);
    // gridLayout->addWidget(isMultiPilotLabel, m_currentRow, m_r6, SINGLE_SPAN, SINGLE_SPAN);
    // gridLayout->addWidget(isPilotFlyingLabel, m_currentRow, m_r7, SINGLE_SPAN, SINGLE_SPAN);
    // gridLayout->addWidget(pilotFunctionLabel, m_currentRow, m_r8, SINGLE_SPAN, SINGLE_SPAN);
    gridLayout->addWidget(startLabel, m_currentRow, m_r1, Qt::AlignLeft);
    gridLayout->addWidget(endLabel, m_currentRow, m_r2, Qt::AlignLeft);
    gridLayout->addWidget(isIfrLabel, m_currentRow, m_r3, Qt::AlignCenter);
    gridLayout->addWidget(isSimIfrLabel, m_currentRow, m_r4, Qt::AlignCenter);
    gridLayout->addWidget(isNightLabel, m_currentRow, m_r5, Qt::AlignCenter);
    gridLayout->addWidget(isMultiPilotLabel, m_currentRow, m_r6, Qt::AlignCenter);
    gridLayout->addWidget(isPilotFlyingLabel, m_currentRow, m_r7, Qt::AlignCenter);
    gridLayout->addWidget(pilotFunctionLabel, m_currentRow, m_r8, Qt::AlignLeft);
    m_currentRow++;

    // Always start with one row of edit widgets
    addRow();

    // Bottom controls

    gridLayout->addWidget(addSegmentButton, m_currentRow + 1, m_r8);
    gridLayout->addWidget(buttonBox, m_currentRow + 2, m_r8);

    gridLayout->setColumnStretch(m_r1, 1); // start
    gridLayout->setColumnStretch(m_r2, 1); // end
    gridLayout->setColumnStretch(m_r3, 0);
    gridLayout->setColumnStretch(m_r4, 0);
    gridLayout->setColumnStretch(m_r5, 0);
    gridLayout->setColumnStretch(m_r6, 0);
    gridLayout->setColumnStretch(m_r7, 0);
    gridLayout->setColumnStretch(m_r8, 1); // function
}

void FlightSegmentEditorWidget::addRow()
{
    SegmentRow row;

    row.startEdit             = new QTimeEdit(this);
    row.endEdit               = new QTimeEdit(this);
    row.isIfrCheckBox         = new QCheckBox(this);
    row.isSimIfrCheckBox      = new QCheckBox(this);
    row.isNightCheckBox       = new QCheckBox(this);
    row.isMultiPilotCheckBox  = new QCheckBox(this);
    row.isPilotFlyingCheckBox = new QCheckBox(this);
    row.pilotFunctionComboBox = new QComboBox(this);

    row.startEdit->setDisplayFormat(m_timeFormatString);
    row.endEdit->setDisplayFormat(m_timeFormatString);
    OPL::GLOBALS->loadPilotFunctions(row.pilotFunctionComboBox);

    int r = m_currentRow;

    gridLayout->addWidget(row.startEdit, r, m_r1, Qt::AlignLeft);
    gridLayout->addWidget(row.endEdit, r, m_r2, Qt::AlignLeft);
    gridLayout->addWidget(row.isIfrCheckBox, r, m_r3, Qt::AlignCenter);
    gridLayout->addWidget(row.isSimIfrCheckBox, r, m_r4, Qt::AlignCenter);
    gridLayout->addWidget(row.isNightCheckBox, r, m_r5, Qt::AlignCenter);
    gridLayout->addWidget(row.isMultiPilotCheckBox, r, m_r6, Qt::AlignCenter);
    gridLayout->addWidget(row.isPilotFlyingCheckBox, r, m_r7, Qt::AlignCenter);
    gridLayout->addWidget(row.pilotFunctionComboBox, r, m_r8, Qt::AlignLeft);

    m_rows.push_back(row);
    m_currentRow++;
}

void FlightSegmentEditorWidget::loadSegments(const QList<OPL::FlightSegmentEntry> &segments)
{
    // Ensure enough rows exist
    while (m_rows.size() < segments.size())
        addRow();

    int uiRow = 0;
    for (int i = segments.size() - 1; i >= 0; --i) {
        const OPL::FlightSegmentEntry &entry = segments[i];
        SegmentRow &row                      = m_rows[uiRow];

        row.startEdit->setTime(QTime::fromMSecsSinceStartOfDay(entry.getStartMs()));
        row.endEdit->setTime(QTime::fromMSecsSinceStartOfDay(entry.getEndMs()));

        ++uiRow;
    }

    // for (int i = 0; i < segments.size(); ++i)
    // {
    //     const OPL::FlightSegmentEntry &entry = segments[i];
    //     SegmentRow &row = m_rows[i];

    //     const QTime startTime = QTime::fromMSecsSinceStartOfDay(entry.getStartMs());
    //     const QTime endTime   = QTime::fromMSecsSinceStartOfDay(entry.getEndMs());

    //     row.startEdit->setTime(startTime);
    //     row.endEdit->setTime(endTime);
    // }
}
