#pragma once
#include <string>
#include <memory>
class Command
{
public:
  explicit Command(long command_id):
    commandId_(command_id)
  {

  }
  std::string topic_;
  std::string payload_;
  std::string replayTopic_;
  long commandId_ = 0;
};

typedef std::shared_ptr<Command> CommandPtr;