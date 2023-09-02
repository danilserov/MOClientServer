#include <iostream>
#include <fstream>
#include "CommandEmitter.h"
using namespace std::chrono_literals;

CommandEmitter::CommandEmitter(const std::string& client_id):
  stopRequested_(false),
  client_id_(client_id),
  pubSubServer_(PubSubServer::getInstance())
{
  pubSubServer_->
    Subscribe(client_id_, static_cast<ISubscriber*>(this));

  thread_ = 
    std::thread(&CommandEmitter::Work, this);
}

CommandEmitter::~CommandEmitter()
{
  pubSubServer_->
    Unsubscribe(client_id_, static_cast<ISubscriber*>(this));
  Stop();
}

void CommandEmitter::OnReceive(CommandPtr command)
{

}

void CommandEmitter::Work()
{
  while (!stopRequested_)
  {
    CommandPtr command(new Command);
    command->replayTopic_ = client_id_;
    command->todo_ = "TODO";
    command->topic_ = PubSubServer::TOPIC_COMMAND;
    pubSubServer_->Publish(command);
    std::this_thread::sleep_for(1ms);
  }
}

void CommandEmitter::Stop()
{
  stopRequested_ = true;
  thread_.join();
}