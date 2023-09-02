#pragma once

#include "PubSubServer.h"

PubSubServerPtr PubSubServer::instance_ = nullptr;

void PubSubServer::CustomDeleter(PubSubServer* ptr)
{
  std::cout << "Custom deleter called" << std::endl;
  delete ptr;
}

PubSubServerPtr PubSubServer::getInstance()
{
  if (!instance_)
  {
    instance_.reset(new PubSubServer(), PubSubServer::CustomDeleter);
  }
  return instance_;
}
