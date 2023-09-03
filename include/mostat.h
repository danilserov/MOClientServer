#pragma once

#include <atomic>

class MOStat
{
public:
  static std::atomic_long sent_;
  static std::atomic_long received_;
  static std::atomic_long publishedQueue_;
  static std::atomic_long servers_;
  static std::atomic_long procQueue_;

  static std::atomic_long maxAsyncTime_;
  static std::atomic_long maxSyncTime_;
};