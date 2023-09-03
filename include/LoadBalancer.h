#pragma once

#include "PubSubServer.h"
#include "CommandProcessor.h"
#include "ISubScriber.h"
#include <shared_mutex>

class LoadBalancer: public ISubscriber
{
private:
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