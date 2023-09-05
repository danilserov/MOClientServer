#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <atomic>

class Command;
typedef std::shared_ptr<Command> CommandPtr;

class Command
{
private:
  static std::atomic_int idGenerator_;
  int commandId_;
public:
  enum E_COMMAND
  {
    TODO_SIN = 1,
    TODO_COS = 2
  };

  Command();

  E_COMMAND commandType;
  double payload_;
  
  int clientId_;
  long long timestamp_;
  bool highPrior_;

  static long long GetTimeStamp();
  CommandPtr CreateReplay(const double payload);
  int GetCommandId();
};


