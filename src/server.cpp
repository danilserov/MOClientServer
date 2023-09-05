
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

    MOStat::procQueue_ = procQueue;
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

void Server::OnReceive(CommandPtr command)
{
  auto proc = GetAvailableProc();
  proc->AddCommand(command);

  if (proc->GetBusyScore() > NORMAL_BUSY_SCORE)
  {
    //As a general rule, we need to create a better waiting mechanism here. But this will do the job.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void Server::OnAnswerReady(CommandPtr command)
{

}
void Server::Stop()
{
  std::unique_lock<std::shared_timed_mutex> lock(mutexProc_);
  for (auto it = commandProcesseros_.begin(); it != commandProcesseros_.end(); ++it)
  {
    (*it)->Stop();
  }
}