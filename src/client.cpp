#include <iostream>
#include <fstream>
#include <stdexcept>
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

    threadAsyncSend_ = 
      std::thread(&Client::AsyncSendThread, this);
  }  
}

Client::~Client()
{
  Stop();
}

int Client::GetClientId() const
{
  return client_id_;
}

void Client::Send(CommandPtr command)
{
  MOStat::sent_++;
  server_->ExecuteCommand(command);
}

void Client::Post(CommandPtr command)
{
  std::lock_guard<std::mutex> lock(mutexAsyncResult_);
  asyncQueue_.push(command);
}

CommandPtr Client::ExecuteSync(CommandPtr command)
{
  CommandPtr result = nullptr;  

  Send(command);

  syncCommandId_ = command->GetCommandId();

  std::unique_lock<std::mutex> lock(mutexSyncSend_);
  auto timeout = std::chrono::seconds(SYNC_SEND_TIMEOUT_SEC);

  conditionSyncReceived_.wait_for(lock, timeout, [this] {
    return syncResult_!= nullptr || stopRequested_;
    });

  std::swap(result, syncResult_);

  return result;
}

void Client::AsyncSendThread()
{
  while (!stopRequested_)
  {
    std::queue<CommandPtr> asyncQueue;
    {
      std::lock_guard<std::mutex> lock(mutexAsyncResult_);
      std::swap(asyncQueue_, asyncQueue);
    }

    if (asyncQueue.empty())
    {
      std::this_thread::sleep_for(1ms);
      continue;
    }

    while (!asyncQueue.empty())
    {
      Send(asyncQueue.front());
      asyncQueue.pop();
    }
  }
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
      auto duration = Command::GetCurTimeStamp() - (*it)->GetTimeStamp();

      if ((*it)->GetCommandId() == syncCommandId_)
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
        results_[(*it)->GetCommandId()] = (*it);
      }
    }
    conditionAsyncReceived_.notify_all();
  }
}

double Client::Sin(double a)
{
  CommandPtr command(new Command(GetClientId(), true));
  command->SetPayload(a);
  command->commandType = Command::TODO_SIN;
  auto result = ExecuteSync(command);

  if (result == nullptr)
  {
    throw std::runtime_error("wrong result, possible timeout");
  }

  return result->GetPayload();
}

double Client::Cos(double a)
{
  CommandPtr command(new Command(GetClientId(), true));
  command->SetPayload(a);
  command->commandType = Command::TODO_COS;
  auto result = ExecuteSync(command);

  if (result == nullptr)
  {
    throw std::runtime_error("wrong result");
  }

  return result->GetPayload();
}

int Client::SinAsync(double a)
{
  CommandPtr command(new Command(GetClientId(), false));
  command->SetPayload(a);
  command->commandType = Command::TODO_SIN;
  Post(command);
  return command->GetCommandId();
}
int Client::CosAsync(double a)
{
  CommandPtr command(new Command(GetClientId(), false));
  command->SetPayload(a);
  command->commandType = Command::TODO_COS;
  Post(command);
  return command->GetCommandId();
}

bool Client::DoGetResult(int command_id, double& result)
{
  std::lock_guard<std::mutex> lock(mutexAsyncResult_);
  auto it = results_.find(command_id);

  if (it != results_.end())
  {    
    result = it->second->GetPayload();
    results_.erase(it);
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

std::vector<int> Client::GetAvailableResultsIds()
{
  std::vector<int> results;
  std::unique_lock<std::mutex> lock(mutexAsyncResult_);

  for (auto it = results_.begin(); it != results_.end(); it++)
  {
    results.push_back(it->first);
  }
  return results;
}

void Client::Stop()
{
  stopRequested_ = true;
  conditionSyncReceived_.notify_all();

  if (thread_.joinable())
  {
    thread_.join();
  }    

  if (threadAsyncSend_.joinable())
  {
    threadAsyncSend_.join();
  }
}