#pragma once
#include <string>
#include <memory>
struct Command
{
  std::string topic_;
  std::string payload_;
  std::string replayTopic_;
};

typedef std::shared_ptr<Command> CommandPtr;