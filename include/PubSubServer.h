#pragma once

#include<mutex>
#include<string>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "command.h"
#include "isubscriber.h"
#include "mostat.h"

class PubSubServer;
typedef std::shared_ptr<PubSubServer> PubSubServerPtr;

class PubSubServer
{
public:
  static const std::string TOPIC_COMMAND;
  static const int MAX_QUEUE;
  static const int APROX_SERVER_DELAY;
private:
  static PubSubServerPtr instance_;
  PubSubServer();
  ~PubSubServer();
  std::unordered_map<std::string, std::unordered_set<ISubscriber*>> subscribers_;
  std::shared_timed_mutex mutexSubscribers_;
  std::vector<std::thread> threads_;
  std::atomic<bool> stopRequested_;

  std::condition_variable conditionQueue_;
  std::queue<CommandPtr> commandQueue_;
  std::mutex mutexQueue_;

  static void CustomDeleter (PubSubServer* ptr);
  void Work();
public:
  static PubSubServerPtr getInstance();
  void Publish(CommandPtr command);
  void Subscribe(const std::string& topic, ISubscriber* subscriber);
  void Unsubscribe(const std::string& topic, ISubscriber* subscriber);
  void Stop();
};
