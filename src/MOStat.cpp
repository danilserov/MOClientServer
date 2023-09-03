#include "MOStat.h"

std::atomic<long> MOStat::sent_ = std::atomic_int(0);
std::atomic<long> MOStat::received_ = std::atomic_int(0);
std::atomic<long> MOStat::publishedQueue_ = std::atomic_int(0);
std::atomic<long> MOStat::servers_ = std::atomic_int(0);
std::atomic<long> MOStat::procQueue_ = std::atomic_int(0);
