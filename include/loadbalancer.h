#pragma once

#include <shared_mutex>

#include "pubsubserver.h"
#include "commandprocessor.h"
#include "isubscriber.h"

class LoadBalancer: public ISubscriber
{
private:
  const int NORMAL_BUSY_SCORE = 50;
  std::vector<CommandProcessorPtr> commandProcesseros_;
  std::shared_timed_mutex mutexProc_;
private:
  std::shared_ptr<PubSubServer> pubSubServer_;
  void OnReceive(CommandPtr command) override;
  CommandProcessorPtr GetAvailableProc();
public:
  LoadBalancer();
  ~LoadBalancer();
  void Stop();
};

typedef std::shared_ptr<LoadBalancer> LoadBalancerPtr;