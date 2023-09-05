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
  long long timestamp_;
  int clientId_;
  bool highPrior_;
  double payload_;
public:
  enum E_COMMAND
  {
    TODO_SIN = 1,
    TODO_COS = 2
  };

  Command(int clientId, bool highPrior);

  E_COMMAND commandType;  

  static long long GetCurTimeStamp();
  CommandPtr CreateReplay(const double payload);
  int GetCommandId();
  int GetClientId();
  long long GetTimeStamp();
  bool IsHighPrior();
  void SetPayload(double a);
  double GetPayload();
};


