#include "opl.h"

namespace OPL {

void OplGlobals::fillLanguageComboBox(QComboBox *combo_box) const
{
    QSignalBlocker blocker(combo_box);
    for (const auto &language : L10N_DisplayNames)
        combo_box->addItem(language);
}

void OplGlobals::fillViewNamesComboBox(QComboBox *combo_box) const
{
    const QSignalBlocker blocker(combo_box);
    for (const auto &view_name : DATABASE_VIEW_DISPLAY_NAMES)
        combo_box->addItem(view_name);
}

void OplGlobals::loadPilotFunctios(QComboBox *combo_box) const
{
    const QSignalBlocker blocker(combo_box);
    for (const auto& pilot_function : PILOT_FUNCTIONS)
        combo_box->addItem(pilot_function);
}

void OplGlobals::loadSimulatorTypes(QComboBox *combo_box) const
{
    const QSignalBlocker blocker(combo_box);
    for (const auto &sim_type : SIMULATOR_TYPES)
        combo_box->addItem(sim_type);
}

void OplGlobals::loadApproachTypes(QComboBox *combo_box) const
{
    const QSignalBlocker blocker(combo_box);
    for (const auto &approach : APPROACH_TYPES)
        combo_box->addItem(approach);
}

} // namespace Opl
