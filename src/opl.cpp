#include "opl.h"

namespace Opl {

void OplGlobals::fillLanguageComboBox(QComboBox *combo_box) const
{
    QSignalBlocker blocker(combo_box);
    for (const auto &language : L10N_DisplayNames)
        combo_box->addItem(language);
}

void OplGlobals::fillViewNamesComboBox(QComboBox *combo_box) const
{
    const QSignalBlocker blocker(combo_box);
    for (int i = 0; i < DATABASE_VIEW_DISPLAY_NAMES.size(); i++)
        combo_box->addItem(DATABASE_VIEW_DISPLAY_NAMES.value(DbViewName(i)));
}

void OplGlobals::loadPilotFunctios(QComboBox *combo_box) const
{
    const QSignalBlocker blocker(combo_box);
    for (int i = PilotFunction::PIC; i < PILOT_FUNCTIONS.size(); i++)
        combo_box->addItem(PILOT_FUNCTIONS.value(PilotFunction(i)));
}

void OplGlobals::loadSimulatorTypes(QComboBox *combo_box) const
{
    const QSignalBlocker blocker(combo_box);
    for (int i = 0; i < SIMULATOR_TYPES.size(); i++)
        combo_box->addItem(SIMULATOR_TYPES.value(SimulatorTypes(i)));
}

void OplGlobals::loadApproachTypes(QComboBox *combo_box) const
{
    const QSignalBlocker blocker(combo_box);
    for (const auto &approach : APPROACH_TYPES)
        combo_box->addItem(approach);
}

} // namespace Opl
