#include "easaftl.h"

int EasaFTL::getLimit(OPL::Statistics::TimeFrame timeFrame)
{
    switch (timeFrame) {
    case OPL::Statistics::TimeFrame::Rolling28Days:
        return 100*60; // 100h
    case OPL::Statistics::TimeFrame::Rolling12Months:
        return 1000 * 60; // 1000h
    case OPL::Statistics::TimeFrame::CalendarYear:
        return 900 * 60; // 900h
    default:
        return 0;
        break;
    }
}
