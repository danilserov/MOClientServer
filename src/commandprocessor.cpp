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

int CommandProcessor::GetBusyScore() const
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
    std::queue<CommandPtr> commandQueue;

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
      std::swap(commandQueue, commandQueue_);
    }

    while (!commandQueue.empty())
    {
      auto replay = ProcessCommand(commandQueue.front());
      commandQueue.pop();
      pubSubServer_->Publish(replay);
    }
  }
}

CommandPtr CommandProcessor::ProcessCommand(CommandPtr command) const
{
  // Simulate some processing.
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  std::string replayPayload = "UNKNOWN_COMMAND_RECEIVED";

  if (command->payload_ == "TODO")
  {
    replayPayload = "REPLAY_TODO";    
  }
  else if (command->payload_ == "SYNC_TODO")
  {
    replayPayload = "REPLAY_SYNC_TODO";
  }
  
  CommandPtr replay = command->CreateReplay(replayPayload);
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

