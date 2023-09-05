#pragma once

#include <string>
#include <vector>
#include <thread>
#include <condition_variable>

#include "server.h"
#include "command.h"

class Client
{
private:
  std::string client_id_;
  std::thread thread_;
  ServerPtr server_;
  std::atomic<long> curCommandId;
public:
  const int SYNC_SEND_TIMEOUT_SEC = 10;
  Client(const std::string& client_id);
  ~Client();
  void Stop();
  double Sin(double a);
  double Cos(double a);
  int SinAsync(double a);
  int CosAsync(double a);
  double GetResult(int command_id);
  CommandPtr ExecuteSync(CommandPtr command);
private:  
  void Publish(CommandPtr command);
  bool stopRequested_;
  void Work();
  void OnReceive(CommandPtr command);  
private:
  std::condition_variable conditionSyncReceived_;
  std::atomic<long> syncCommandId_;
  CommandPtr syncResult_;
  std::mutex mutexSyncSend_;
};

typedef std::shared_ptr<Client> ClientPtr;