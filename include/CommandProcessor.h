#pragma once
#include <string>
#include <queue>
#include <future>
#include "PubSubServer.h"

class CommandProcessor
{
private:
  std::shared_ptr<PubSubServer> PubSubServer_;
public:
  explicit CommandProcessor();
};
