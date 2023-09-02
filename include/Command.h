#pragma once
#include <string>
#include <memory>
struct Command
{
  std::string topic_;
  std::string todo_;
  std::string replayTopic_;
  std::string result_;
};

typedef std::shared_ptr<Command> CommandPtr;