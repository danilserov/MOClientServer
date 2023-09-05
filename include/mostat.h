#pragma once

#include <atomic>

class MOStat
{
public:
  static std::atomic_int sent_;
  static std::atomic_int received_;
  static std::atomic_int servers_;

  static std::atomic_int maxAsyncTime_;
  static std::atomic_int maxSyncTime_;
};