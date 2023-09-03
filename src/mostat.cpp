#include "mostat.h"

std::atomic_long MOStat::sent_(0);
std::atomic_long MOStat::received_ (0);
std::atomic_long MOStat::publishedQueue_ (0);
std::atomic_long MOStat::servers_ (0);
std::atomic_long MOStat::procQueue_ (0);

std::atomic_long MOStat::maxAsyncTime_(0);
std::atomic_long MOStat::maxSyncTime_(0);
