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
};