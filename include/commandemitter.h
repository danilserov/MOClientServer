#pragma once

#include <string>
#include <vector>
#include <thread>
#include <condition_variable>

#include "pubsubserver.h"
#include "isubscriber.h"
#include "command.h"

class CommandEmitter: public ISubscriber
{
private:
  std::string client_id_;
  std::thread thread_;
  std::shared_ptr<PubSubServer> pubSubServer_;
  std::atomic<long> curCommandId;
public:
  const int SYNC_SEND_TIMEOUT_SEC = 10;
  CommandEmitter(const std::string& client_id);
  ~CommandEmitter();
  void Stop();
  CommandPtr ExecuteSync(CommandPtr command);
private:
  bool stopRequested_;
  void Work();
  void OnReceive(CommandPtr command) override;  
private:
  std::condition_variable conditionSyncReceived_;
  std::atomic<long> syncCommandId_;
  CommandPtr syncResult_;
  std::mutex mutexSyncSend_;
};

typedef std::shared_ptr<CommandEmitter> CommandEmitterPtr;