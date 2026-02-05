#ifndef SPAWRAPPER_H
#define SPAWRAPPER_H
extern "C" {
#include "src/calc/spa.h"
}

/*!
 * \brief wraps the SPA c library to avoid namespace pollution
 */
namespace SPA {

using data = spa_data;

inline int calculate(data* spa) {
    return spa_calculate(spa);
}

} // namespace SPA

#endif // SPAWRAPPER_H
