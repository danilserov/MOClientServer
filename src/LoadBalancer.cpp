
#include "LoadBalancer.h"

LoadBalancer::LoadBalancer() :
  pubSubServer_(PubSubServer::getInstance())
{
  pubSubServer_->
    Subscribe(PubSubServer::TOPIC_COMMAND, static_cast<ISubscriber*>(this));
}

LoadBalancer::~LoadBalancer()
{
  pubSubServer_->
    Unsubscribe(PubSubServer::TOPIC_COMMAND, static_cast<ISubscriber*>(this));
}

void LoadBalancer::OnReceive(CommandPtr command)
{

}