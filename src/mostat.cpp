#include "mostat.h"

std::atomic_int MOStat::sent_(0);
std::atomic_int MOStat::received_ (0);
std::atomic_int MOStat::servers_ (0);

std::atomic_int MOStat::maxAsyncTime_(0);
std::atomic_int MOStat::maxSyncTime_(0);
