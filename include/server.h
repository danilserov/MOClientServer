#pragma once

#include <shared_mutex>
#include <memory>
#include <unordered_map>
#include "commandprocessor.h"

class Server;
typedef std::shared_ptr<Server> ServerPtr;

class Server
{
  friend class CommandProcessor;
private:
  static ServerPtr instance_;
  Server();
  ~Server();
  static void CustomDeleter(Server* ptr);
  const int NORMAL_BUSY_SCORE = 50;
  std::vector<CommandProcessorPtr> commandProcesseros_;
  std::shared_timed_mutex mutexProc_;

  std::mutex mutexResults_;
  std::unordered_map<int, std::vector<CommandPtr>> results_;
  std::unordered_map<int, std::shared_ptr<std::condition_variable>> resultsReadyCond_;

  std::shared_ptr<std::condition_variable> GetClientWaiter(int client_id);
private:  
  CommandProcessorPtr GetAvailableProc();
  void OnAnswerReady(CommandPtr command);
public:  
  static ServerPtr getInstance();
  void Stop();
  void ExecuteCommand(CommandPtr command);
  std::vector<CommandPtr> GetReadyResults(int client_id);
};

