#include <iostream>
#include <fstream>
#include "client.h"
#include "mostat.h"

using namespace std::chrono_literals;


Client::Client(const std::string& client_id):
  stopRequested_(false),
  client_id_(client_id),
  curCommandId(0),
  syncResult_(nullptr),
  server_(Server::getInstance())
{
  if (client_id != "test_sync_client")
  {
    thread_ =
      std::thread(&Client::Work, this);
  }  
}

Client::~Client()
{
  Stop();
}

void Client::OnReceive(CommandPtr command)
{
  MOStat::received_++;

  if (syncCommandId_ == command->commandId_)
  {
    syncResult_ = command;
    conditionSyncReceived_.notify_one();
  }

  auto duration = Command::GetTimeStamp() - command->timestamp_;
}

void Client::Publish(CommandPtr command)
{
  MOStat::sent_++;
}

CommandPtr Client::ExecuteSync(CommandPtr command)
{
  CommandPtr result = nullptr;  

  command->payload_ = 50;
  syncCommandId_ = command->commandId_;
  Publish(command);

  std::unique_lock<std::mutex> lock(mutexSyncSend_);
  auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(SYNC_SEND_TIMEOUT_SEC);

  conditionSyncReceived_.wait_until(lock, timeout, [this] {
    return syncResult_!= nullptr || stopRequested_;
    });

  std::swap(result, syncResult_);

  return result;
}

void Client::Work()
{
  while (!stopRequested_)
  {
    CommandPtr command(new Command(curCommandId++));

    command->payload_ = 50;
    command->topic_ = Command::TODO_SIN;
    
    if (MOStat::sent_ % 100 == 0)
    {
      ExecuteSync(command);
      Publish(command);
    }
    else
    {
      Publish(command);
    }
  }
}

double Client::Sin(double a)
{
  return 0;
}
double Client::Cos(double a)
{
  return 0;
}
int Client::SinAsync(double a)
{
  return 0;
}
int Client::CosAsync(double a)
{
  return 0;
}

double Client::GetResult(int command_id)
{
  return 0;
}

void Client::Stop()
{
  stopRequested_ = true;
  conditionSyncReceived_.notify_all();

  if (thread_.joinable())
    thread_.join();
}