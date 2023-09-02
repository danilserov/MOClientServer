#pragma once

#include<mutex>
#include<string>
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include "Command.h"
#include "ISubscriber.h"
#include "MOStat.h"

class PubSubServer;
typedef std::shared_ptr<PubSubServer> PubSubServerPtr;

class PubSubServer
{
public:
  static const std::string TOPIC_COMMAND;
  static const int MAX_QUEUE = 10000;
private:
  static PubSubServerPtr instance_;
  PubSubServer();
  ~PubSubServer();
  std::unordered_map<std::string, std::unordered_set<ISubscriber*>> subscribers_;
  std::shared_mutex mutexSubscribers_;
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
