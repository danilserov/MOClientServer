#include <chrono>
#include <iostream>
#include <fstream>
#include <queue>

#include "commandprocessor.h"
#include "pubsubserver.h"


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
  std::lock_guard<std::mutex> lock(mutexQueue_);
  return (int)commandQueue_.size();
}

void CommandProcessor::Stop()
{
  stopRequested_ = true;
  conditionQueue_.notify_one();

  if (thread_.joinable())
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
      auto replay = ProcessCommand(command);
      pubSubServer_->Publish(replay);
    }
  }
}

CommandPtr CommandProcessor::ProcessCommand(CommandPtr command)
{
  CommandPtr replay(new Command(command->commandId_));
  replay->topic_ = command->replayTopic_;

  if (command->payload_ == "TODO")
  {
    replay->payload_ = "REPLAY_TODO";
    // Simulate some processing.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  else if (command->payload_ == "SYNC_TODO")
  {
    replay->payload_ = "REPLAY_SYNC_TODO";
  }
  else
  {
    replay->payload_ = "UNKNOWN_COMMAND_RECEIVED";
  }
  
  replay->timestamp_ = command->timestamp_;
  return replay;
}

void CommandProcessor::AddCommand(CommandPtr command)
{
  {
    std::lock_guard<std::mutex> lock(mutexQueue_);
    commandQueue_.push(command);
  }

  conditionQueue_.notify_one();
}

