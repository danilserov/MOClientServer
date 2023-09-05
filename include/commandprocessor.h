#pragma once

#include <string>
#include <queue>
#include <memory>
#include <thread>
#include <condition_variable>
#include "command.h"

class Server;
typedef std::shared_ptr<Server> ServerPtr;

class CommandProcessor
{
  friend class CommandProcessorDecorator;
private:
  ServerPtr parentServer_;
  std::thread thread_;
  bool stopRequested_;
  void Work();

  std::condition_variable conditionQueue_;
  std::queue<CommandPtr> commandQueue_;
  mutable std::mutex mutexQueue_;
  CommandPtr ProcessCommand(CommandPtr command) const;
public:
  CommandProcessor();
  ~CommandProcessor();
  int GetBusyScore() const;
  void Stop();
  void AddCommand(CommandPtr command);
};

class CommandProcessorDecorator
{
  // Class for testing function in the gtest.
public:
  CommandPtr ProcessCommand(CommandPtr command) const
  {
    CommandProcessor cp;
    return cp.ProcessCommand(command);
  }
};
typedef std::shared_ptr<CommandProcessor> CommandProcessorPtr;
