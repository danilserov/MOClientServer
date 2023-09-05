#pragma once

#include <shared_mutex>
#include <memory>

#include "commandprocessor.h"

class Server;
typedef std::shared_ptr<Server> ServerPtr;

class Server
{
private:
  static ServerPtr instance_;
  Server();
  ~Server();
  static void CustomDeleter(Server* ptr);
  const int NORMAL_BUSY_SCORE = 50;
  std::vector<CommandProcessorPtr> commandProcesseros_;
  std::shared_timed_mutex mutexProc_;
private:
  void OnReceive(CommandPtr command);
  CommandProcessorPtr GetAvailableProc();
public:
  static ServerPtr getInstance();
  void OnAnswerReady(CommandPtr command);
  void Stop();
};

