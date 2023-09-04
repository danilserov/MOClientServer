
#include "loadbalancer.h"

LoadBalancer::LoadBalancer() :
  pubSubServer_(PubSubServer::getInstance())
{
  pubSubServer_->
    Subscribe(PubSubServer::TOPIC_COMMAND, static_cast<ISubscriber*>(this));
}

LoadBalancer::~LoadBalancer()
{
  pubSubServer_->
    Unsubscribe(PubSubServer::TOPIC_COMMAND, static_cast<ISubscriber*>(this));
}

CommandProcessorPtr LoadBalancer::GetAvailableProc()
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

void LoadBalancer::OnReceive(CommandPtr command)
{
  auto proc = GetAvailableProc();
  proc->AddCommand(command);

  if (proc->GetBusyScore() > NORMAL_BUSY_SCORE)
  {
    //As a general rule, we need to create a better waiting mechanism here. But this will do the job.
    std::this_thread::sleep_for(std::chrono::milliseconds(PubSubServer::APROX_SERVER_DELAY));
  }
}

void LoadBalancer::Stop()
{
  std::unique_lock<std::shared_timed_mutex> lock(mutexProc_);
  for (auto it = commandProcesseros_.begin(); it != commandProcesseros_.end(); ++it)
  {
    (*it)->Stop();
  }
}