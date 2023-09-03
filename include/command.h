#pragma once

#include <string>
#include <memory>
#include <chrono>

class Command
{
public:
  explicit Command(long command_id):
    commandId_(command_id)
  {
    timestamp_ = GetTimeStamp();
  }
  std::string topic_;
  std::string payload_;
  std::string replayTopic_;
  long commandId_ = 0;
  long long timestamp_;

  static long long GetTimeStamp()
  {
    auto current_time = std::chrono::system_clock::now();
    std::chrono::milliseconds timestamp =
      std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch());
    return timestamp.count();
  }
};

typedef std::shared_ptr<Command> CommandPtr;