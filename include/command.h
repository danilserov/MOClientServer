#pragma once

#include <string>
#include <memory>
#include <chrono>

class Command;
typedef std::shared_ptr<Command> CommandPtr;

class Command
{
public:
  enum E_COMMAND
  {
    TODO_SIN,
    TODO_COS
  };

  explicit Command(long command_id):
    commandId_(command_id),
    clientId_(-1)
  {
    timestamp_ = GetTimeStamp();
  }
  E_COMMAND commandType;
  double payload_;
  int commandId_;
  int clientId_;
  long long timestamp_;

  static long long GetTimeStamp()
  {
    auto current_time = std::chrono::system_clock::now();
    std::chrono::milliseconds timestamp =
      std::chrono::duration_cast<std::chrono::milliseconds>(
        current_time.time_since_epoch()
      );
    return timestamp.count();
  }

  CommandPtr CreateReplay(const double payload)
  {
    CommandPtr replay(new Command(commandId_));
    replay->timestamp_ = timestamp_;
    replay->payload_ = payload;
    replay->clientId_ = clientId_;
    return replay;
  }
};
