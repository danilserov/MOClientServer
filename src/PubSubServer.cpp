#pragma once

#include "PubSubServer.h"

const std::string PubSubServer::TOPIC_COMMAND = "command";
PubSubServerPtr PubSubServer::instance_ = nullptr;

void PubSubServer::CustomDeleter(PubSubServer* ptr)
{
  delete ptr;
}

PubSubServer::PubSubServer() :
  stopRequested_(false)
{
  unsigned int numCores = std::thread::hardware_concurrency();

  while (numCores-- != 0)
  {
    threads_.push_back(
      std::thread(&PubSubServer::Work, this)
    );
  }  
}

PubSubServer::~PubSubServer()
{
  Stop();
}

PubSubServerPtr PubSubServer::getInstance()
{
  if (!instance_)
  {
    instance_.reset(new PubSubServer(), PubSubServer::CustomDeleter);
  }
  return instance_;
}

void PubSubServer::Stop()
{
  stopRequested_ = true;
  conditionQueue_.notify_all();

  for (auto it = threads_.begin(); it != threads_.end(); it++)
  {
    it->join();
  }
}

void PubSubServer::Work()
{
  while (!stopRequested_)
  {
    CommandPtr command = nullptr;
    {
      std::unique_lock<std::mutex> lock(mutexQueue_);
      conditionQueue_.wait(lock, [this] {
        return !commandQueue_.empty() || stopRequested_;
        });

      if (commandQueue_.empty())
      {
        continue;
      }
      command = commandQueue_.front();
      commandQueue_.pop();
    }

    if (command != nullptr)
    {
      std::shared_lock<std::shared_mutex> shared_lock(mutexSubscribers_);
      auto it = subscribers_.find(command->topic_);

      if (it != subscribers_.end())
      {
        for (auto itSubscr = it->second.begin(); itSubscr != it->second.end(); itSubscr++)
        {
          (*itSubscr)->OnReceive(command);
        }
      }
    }
  }
}

void PubSubServer::Publish(CommandPtr command)
{
  {
    std::lock_guard<std::mutex> lock(mutexQueue_);
    commandQueue_.push(command);
  }
  
  conditionQueue_.notify_one();
}

void PubSubServer::Subscribe(const std::string& topic, ISubscriber* subscriber)
{
  std::unique_lock<std::shared_mutex>(mutexSubscribers_);

  auto it = subscribers_.find(topic);

  if (it == subscribers_.end())
  {
    subscribers_[topic].insert(subscriber);
  }
  else
  {
    it->second.insert(subscriber);
  }
}

void PubSubServer::Unsubscribe(const std::string& topic, ISubscriber* subscriber)
{
  std::unique_lock<std::shared_mutex>(mutexSubscribers_);

  auto it = subscribers_.find(topic);

  if (it != subscribers_.end())
  {
    it->second.erase(subscriber);
  }
}