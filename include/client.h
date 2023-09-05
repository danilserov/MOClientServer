#pragma once

#include <string>
#include <vector>
#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <atomic>

#include "server.h"
#include "command.h"

class Client
{
private:
  int client_id_;
  std::thread thread_;
  ServerPtr server_;
public:
  const int SYNC_SEND_TIMEOUT_SEC = 10;
  Client(const int client_id);
  ~Client();
  void Stop();
  double Sin(double a);
  double Cos(double a);
  int SinAsync(double a);
  int CosAsync(double a);
  double GetResult(int command_id);
  CommandPtr ExecuteSync(CommandPtr command);
  int GetClientId() const
  {
    return client_id_;
  }
  std::vector<int> GetAvailableResultsIds();
private:
  bool DoGetResult(int command_id, double& result);

  std::unordered_map<int, CommandPtr> results_;
  std::mutex mutexAsyncResult_;
  std::thread threadAsyncSend_;
  std::queue<CommandPtr> asyncQueue_;
  void AsyncSendThread();

  std::condition_variable conditionAsyncReceived_;
  void Send(CommandPtr command);
  void Post(CommandPtr command);
  bool stopRequested_;
  void Work();
private:
  std::condition_variable conditionSyncReceived_;
  std::atomic_int syncCommandId_;
  CommandPtr syncResult_;
  std::mutex mutexSyncSend_;
};

typedef std::shared_ptr<Client> ClientPtr;