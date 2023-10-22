#ifndef EASAFTL_H
#define EASAFTL_H
#include "src/functions/statistics.h"


class EasaFTL
{
public:
    EasaFTL() = delete;

    static int getLimit(OPL::Statistics::TimeFrame timeFrame);
};

#endif // EASAFTL_H
