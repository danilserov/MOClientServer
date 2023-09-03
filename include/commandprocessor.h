#pragma once

#include <string>
#include <queue>

#include "pubsubserver.h"

class CommandProcessor
{
private:
  std::shared_ptr<PubSubServer> pubSubServer_;
  std::thread thread_;
  bool stopRequested_;
  void Work();

  std::condition_variable conditionQueue_;
  std::queue<CommandPtr> commandQueue_;
  std::mutex mutexQueue_;
  CommandPtr ProcessCommand(CommandPtr command);
public:
  CommandProcessor();
  ~CommandProcessor();
  int GetBusyScore();
  void Stop();
  void AddCommand(CommandPtr command);
};

typedef std::shared_ptr<CommandProcessor> CommandProcessorPtr;
