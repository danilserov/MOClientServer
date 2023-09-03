#include <iostream>
#include <fstream>
#include "commandemitter.h"
#include "mostat.h"

using namespace std::chrono_literals;


CommandEmitter::CommandEmitter(const std::string& client_id):
  stopRequested_(false),
  client_id_(client_id),
  curCommandId(0),
  syncResult_(nullptr),
  pubSubServer_(PubSubServer::getInstance())
{
  pubSubServer_->
    Subscribe(client_id_, static_cast<ISubscriber*>(this));

  if (client_id != "test_sync_client")
  {
    thread_ =
      std::thread(&CommandEmitter::Work, this);
  }  
}

CommandEmitter::~CommandEmitter()
{
  pubSubServer_->
    Unsubscribe(client_id_, static_cast<ISubscriber*>(this));
  Stop();
}

void CommandEmitter::OnReceive(CommandPtr command)
{
  MOStat::received_++;

  if (syncCommandId_ == command->commandId_)
  {
    syncResult_ = command;
    conditionSyncReceived_.notify_one();
  }

  auto duration = Command::GetTimeStamp() - command->timestamp_;

  if (command->payload_ == "REPLAY_TODO")
  {    
    if (MOStat::maxAsyncTime_ < duration)
    {
      MOStat::maxAsyncTime_ = duration;
    }
  }

  if (command->payload_ == "REPLAY_SYNC_TODO")
  {
    if (MOStat::maxSyncTime_ < duration)
    {
      MOStat::maxSyncTime_ = duration;
    }
  }
}

CommandPtr CommandEmitter::ExecuteSync(CommandPtr command)
{
  CommandPtr result = nullptr;  

  command->replayTopic_ = client_id_;
  command->payload_ = "SYNC_TODO";
  syncCommandId_ = command->commandId_;
  pubSubServer_->Publish(command);

  std::unique_lock<std::mutex> lock(mutexSyncSend_);
  MOStat::sent_++;  

  auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(SYNC_SEND_TIMEOUT_SEC);

  conditionSyncReceived_.wait_until(lock, timeout, [this] {
    return syncResult_!= nullptr || stopRequested_;
    });

  std::swap(result, syncResult_);

  return result;
}

void CommandEmitter::Work()
{
  while (!stopRequested_)
  {
    CommandPtr command(new Command(curCommandId++));
    command->replayTopic_ = client_id_;
    command->payload_ = "TODO";
    command->topic_ = PubSubServer::TOPIC_COMMAND;
    
    MOStat::sent_++;

    if (MOStat::sent_ % 10 == 0)
    {
      ExecuteSync(command);
    }
    else
    {
      pubSubServer_->Publish(command);
    }
    //std::this_thread::sleep_for(1ms);
  }
}

void CommandEmitter::Stop()
{
  stopRequested_ = true;
  conditionSyncReceived_.notify_all();

  if (thread_.joinable())
    thread_.join();
}