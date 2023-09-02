#include "MOStat.h"

std::atomic<long> MOStat::sent_ = 0;
std::atomic<long> MOStat::received_ = 0;
std::atomic<long> MOStat::publishedQueue_ = 0;
std::atomic<long> MOStat::servers_ = 0;
std::atomic<long> MOStat::procQueue_ = 0;
