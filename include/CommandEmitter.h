#pragma once

#include <string>
#include <vector>
#include "PubSubServer.h"

class CommandEmitter
{
public:
  CommandEmitter();
  ~CommandEmitter();
private:
};

typedef std::shared_ptr<CommandEmitter> CommandEmitterPtr;