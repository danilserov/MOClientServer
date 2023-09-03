#pragma once

#include "command.h"

struct ISubscriber
{
  virtual void OnReceive(CommandPtr command) = 0;
};