#pragma once
#include "Command.h"

struct ISubscriber
{
  virtual void OnReceive(CommandPtr command) = 0;
};