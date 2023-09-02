#pragma once
#include <string>
#include <queue>
#include <future>
#include "PubSubServer.h"

class CommandProcessor
{
private:
  std::shared_ptr<PubSubServer> pubSubServer_;
public:
  explicit CommandProcessor();
};

typedef std::shared_ptr<CommandProcessor> CommandProcessorPtr;
