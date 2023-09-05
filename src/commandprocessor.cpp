#include <chrono>
#include <iostream>
#include <fstream>
#include <queue>

#include "commandprocessor.h"
#include "server.h"


CommandProcessor::CommandProcessor():
  parentServer_(Server::getInstance()),
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
    std::deque<CommandPtr> commandQueue;

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
      commandQueue.pop_front();
      parentServer_->OnAnswerReady(replay);
    }
  }
}

CommandPtr CommandProcessor::ProcessCommand(CommandPtr command) const
{
  // Simulate some processing.
  //std::this_thread::sleep_for(std::chrono::milliseconds(1));

  double replayPayload = 0;

  if (command->commandType == Command::TODO_COS)
  {
    replayPayload = std::cos(command->payload_);    
  }
  else if (command->payload_ == Command::TODO_SIN)
  {
    replayPayload = std::sin(command->payload_);
  }
  
  CommandPtr replay = command->CreateReplay(replayPayload);
  return replay;
}

void CommandProcessor::AddCommand(CommandPtr command)
{
  {
    std::lock_guard<std::mutex> lock(mutexQueue_);

    if (command->highPrior_)
    {
      commandQueue_.push_front(command);
    }
    else
    {
      commandQueue_.push_back(command);
    }    
  }

  conditionQueue_.notify_one();
}

