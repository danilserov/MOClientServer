#pragma once

#include<mutex>
#include<string>
#include <fstream>
#include <iostream>
#include <vector>

class PubSubServer;
typedef std::shared_ptr<PubSubServer> PubSubServerPtr;

class PubSubServer
{
private:
  static PubSubServerPtr instance_;
  PubSubServer() {};
  ~PubSubServer() {};

  static void CustomDeleter (PubSubServer* ptr);
public:
  static PubSubServerPtr getInstance();
};

