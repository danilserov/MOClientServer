#pragma once
#include <string>
#include <memory>
struct Command
{
  std::string topic_;
  std::string payload_;
  std::string replayTopic_;
  int delay_ = 0;
};

typedef std::shared_ptr<Command> CommandPtr;