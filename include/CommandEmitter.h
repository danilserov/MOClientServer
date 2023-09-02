#pragma once

#include <string>
#include <vector>
#include <thread>
#include "PubSubServer.h"
#include "ISubscriber.h"
#include "Command.h"

class CommandEmitter: public ISubscriber
{
private:
  std::string client_id_;
  std::thread thread_;
  std::shared_ptr<PubSubServer> pubSubServer_;

public:
  CommandEmitter(const std::string& client_id);
  ~CommandEmitter();
  void Stop();
private:
  bool stopRequested_;
  void Work();
  void OnReceive(CommandPtr command) override;
};

typedef std::shared_ptr<CommandEmitter> CommandEmitterPtr;