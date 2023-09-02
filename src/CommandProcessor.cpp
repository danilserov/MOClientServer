#include <chrono>
#include <iostream>
#include <fstream>
#include <queue>

#include "CommandProcessor.h"
#include "PubSubServer.h"


CommandProcessor::CommandProcessor():
  pubSubServer_(PubSubServer::getInstance()),
  stopRequested_(false)
{
  thread_ =
    std::thread(&CommandProcessor::Work, this);
}

CommandProcessor::~CommandProcessor()
{
  Stop();
}

int CommandProcessor::GetBusyScore()
{
  return 0;
}

void CommandProcessor::Stop()
{
  stopRequested_ = true;
  conditionQueue_.notify_one();
  thread_.join();
}

void CommandProcessor::Work()
{
  while (!stopRequested_)
  {
    CommandPtr command = nullptr;
    {
      std::unique_lock<std::mutex> lock(mutexQueue_);
      conditionQueue_.wait(lock, [this] {
        return !commandQueue_.empty() || stopRequested_;
        });

      if (commandQueue_.empty())
      {
        continue;
      }
      command = commandQueue_.front();
      commandQueue_.pop();
    }

    if (command != nullptr)
    {
      CommandPtr replay(new Command());
      replay->topic_ = command->replayTopic_;
      replay->payload_ = "REPLAY";
      pubSubServer_->Publish(replay);
    }
  }
}

void CommandProcessor::AddCommand(CommandPtr command)
{
  {
    std::lock_guard<std::mutex> lock(mutexQueue_);
    commandQueue_.push(command);
  }

  conditionQueue_.notify_one();
}

