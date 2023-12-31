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
  const int SYNC_SEND_TIMEOUT_SEC = 10;
public:  
  // Public interface.
  Client(const int client_id);
  ~Client();
  void Stop();
  double Sin(double a);
  double Cos(double a);
  int SinAsync(double a);
  int CosAsync(double a);
  double GetResult(int command_id);
  CommandPtr ExecuteSync(CommandPtr command);
  int GetClientId() const;
  std::vector<int> GetAvailableResultsIds();
private:
  CommandPtr DoGetResult(int command_id);

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
};

typedef std::shared_ptr<Client> ClientPtr;