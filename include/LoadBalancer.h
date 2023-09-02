#pragma once

#include "PubSubServer.h"
#include "CommandProcessor.h"
#include "ISubScriber.h"

class LoadBalancer: public ISubscriber
{
private:
  std::vector<CommandProcessorPtr> commandProcesseros_;
private:
  std::shared_ptr<PubSubServer> pubSubServer_;
  void OnReceive(CommandPtr command) override;
public:
  LoadBalancer();
  ~LoadBalancer();
};

typedef std::shared_ptr<LoadBalancer> LoadBalancerPtr;