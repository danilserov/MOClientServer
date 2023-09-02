#pragma once

#include <atomic>

class MOStat
{
public:
  static std::atomic<long> sent_;
  static std::atomic<long> received_;
  static std::atomic<long> publishedQueue_;
  static std::atomic<long> servers_;
  static std::atomic<long> procQueue_;
};