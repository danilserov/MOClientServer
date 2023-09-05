#include <iostream>
#include <fstream>
#include "client.h"
#include "mostat.h"

using namespace std::chrono_literals;

Client::Client(const int client_id):
  stopRequested_(false),
  client_id_(client_id),
  syncResult_(nullptr),
  syncCommandId_(-1),
  server_(Server::getInstance())
{
  if (client_id >= 0)
  {
    thread_ =
      std::thread(&Client::Work, this);
  }  
}

Client::~Client()
{
  Stop();
}

void Client::Send(CommandPtr command)
{
  command->clientId_ = client_id_;
  command->commandId_ = MOStat::sent_++;
  server_->ExecuteCommand(command);
}

CommandPtr Client::ExecuteSync(CommandPtr command)
{
  CommandPtr result = nullptr;  
   
  command->highPrior_ = true;
  Send(command);

  syncCommandId_ = command->commandId_;

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
    auto results = server_->GetReadyResults(client_id_);

    if (results.empty())
    {
      std::this_thread::sleep_for(1ms);
      continue;
    }

    MOStat::received_ += (int)results.size();

    for (auto it = results.begin(); it != results.end(); it++)
    { 
      auto duration = Command::GetTimeStamp() - (*it)->timestamp_;

      if ((*it)->commandId_ == syncCommandId_)
      {
        syncResult_ = *it;
        conditionSyncReceived_.notify_one();

        if (MOStat::maxSyncTime_ < duration)
        {
          MOStat::maxSyncTime_ = (int)duration;
        }        
      }
      else
      {
        if (MOStat::maxAsyncTime_ < duration)
        {
          MOStat::maxAsyncTime_ = (int)duration;
        }

        std::lock_guard<std::mutex> lock(mutexAsyncResult_);
        results_[(*it)->commandId_] = (*it);        
      }
    }
    conditionAsyncReceived_.notify_all();
  }
}

double Client::Sin(double a)
{
  CommandPtr command = std::make_shared<Command>(0);
  command->payload_ = a;
  command->commandType = Command::TODO_SIN;
  auto result = ExecuteSync(command);

  if (result == nullptr)
  {
    throw::std::exception("wrong result");
  }
  //std::cout << "sin " << a << "=" << result->payload_ << std::endl;
  return result->payload_;
}

double Client::Cos(double a)
{
  CommandPtr command = std::make_shared<Command>(0);
  command->payload_ = a;
  command->commandType = Command::TODO_COS;
  auto result = ExecuteSync(command);

  if (result == nullptr)
  {
    throw::std::exception("wrong result");
  }
  //std::cout << "cos " << a << "=" << result->payload_ << std::endl;
  return result->payload_;
}

int Client::SinAsync(double a)
{
  CommandPtr command = std::make_shared<Command>(0);
  command->payload_ = a;
  command->commandType = Command::TODO_SIN;
  Send(command);
  return command->commandId_;
}
int Client::CosAsync(double a)
{
  CommandPtr command = std::make_shared<Command>(0);
  command->payload_ = a;
  command->commandType = Command::TODO_COS;
  Send(command);
  return command->commandId_;
}

bool Client::DoGetResult(int command_id, double& result)
{
  std::lock_guard<std::mutex> lock(mutexAsyncResult_);
  auto it = results_.find(command_id);

  if (it != results_.end())
  {
    result = it->second->payload_;
    return true;
  }

  return false;
}

double Client::GetResult(int command_id)
{
  double res;

  while (!DoGetResult(command_id, res))
  {
    std::unique_lock<std::mutex> lock(mutexAsyncResult_);
    conditionAsyncReceived_.wait(lock);
  }  
  return res;
}

void Client::Stop()
{
  stopRequested_ = true;
  conditionSyncReceived_.notify_all();

  if (thread_.joinable())
    thread_.join();
}