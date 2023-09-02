#include <chrono>
#include <iostream>
#include <fstream>
#include <queue>

#include "CommandProcessor.h"
#include "PubSubServer.h"


CommandProcessor::CommandProcessor():
  pubSubServer_(PubSubServer::getInstance())
{

}

