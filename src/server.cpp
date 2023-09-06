
#include "server.h"
#include "mostat.h"

ServerPtr Server::instance_ = nullptr;

void Server::CustomDeleter(Server* ptr)
{
  delete ptr;
}

ServerPtr Server::getInstance()
{
  if (!instance_)
  {
    instance_.reset(new Server(), Server::CustomDeleter);
  }
  return instance_;
}

Server::Server()
{

}

Server::~Server()
{

}

CommandProcessorPtr Server::GetAvailableProc()
{
  int max_score_val = 1000000;
  size_t procSize = 0;
  CommandProcessorPtr retVal = nullptr;
  {
    std::shared_lock<std::shared_timed_mutex> shared_lock(mutexProc_);
    procSize = commandProcesseros_.size();
    MOStat::servers_ = (long)procSize;
    int procQueue = 0;

    for (auto it = commandProcesseros_.begin(); it != commandProcesseros_.end(); it++)
    {
      int score = (*it)->GetBusyScore();
      procQueue += score;

      if (score < max_score_val)
      {
        max_score_val = score;
        retVal = *it;
      }
    }
  }

  if (
      (retVal == nullptr || retVal->GetBusyScore() > NORMAL_BUSY_SCORE) &&
      procSize <  std::thread::hardware_concurrency()
  )
  {
    std::unique_lock<std::shared_timed_mutex> lock(mutexProc_);
    retVal = std::make_shared<CommandProcessor>();
    commandProcesseros_.push_back(retVal);    
  }
  return retVal;
}

void Server::ExecuteCommand(CommandPtr command)
{
  auto proc = GetAvailableProc();

  proc->AddCommand(command);

  if (proc->GetBusyScore() > NORMAL_BUSY_SCORE && !command->IsHighPrior())
  {
    //As a general rule, we need to create a better waiting mechanism here. But this will do the job.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

std::shared_ptr<std::condition_variable> Server::GetClientWaiter(int client_id)
{
  std::shared_ptr<std::condition_variable> clientReady = nullptr;
  {
    std::lock_guard<std::mutex> lock(mutexResults_);
    auto condVar = resultsReadyCond_.find(client_id);

    if (condVar != resultsReadyCond_.end())
    {
      clientReady = condVar->second;
    }
    else
    {
      clientReady = std::make_shared<std::condition_variable>();
      resultsReadyCond_[client_id] = clientReady;
    }
  }
  return clientReady;
}

void Server::OnAnswerReady(CommandPtr command)
{
  auto clientReady = GetClientWaiter(command->GetClientId());
  {
    std::lock_guard<std::mutex> lock(mutexResults_);
    results_[command->GetClientId()].push_back(command);
  }

  if (clientReady)
  {
    clientReady->notify_all();
  }  
}

std::vector<CommandPtr> Server::GetReadyResults(int client_id)
{
  {
    auto clientReady = GetClientWaiter(client_id);
    std::unique_lock<std::mutex> lock(mutexResults_);
    auto timeout = std::chrono::milliseconds(10);
    clientReady->wait_for(lock, timeout);
  }

  std::vector<CommandPtr> retVal;
  std::lock_guard<std::mutex> lock(mutexResults_);
  std::swap(results_[client_id], retVal);
  return retVal;
}

void Server::Stop()
{
  std::unique_lock<std::shared_timed_mutex> lock(mutexProc_);
  for (auto it = commandProcesseros_.begin(); it != commandProcesseros_.end(); ++it)
  {
    (*it)->Stop();
  }
}