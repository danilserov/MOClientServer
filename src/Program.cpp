// CommandEmitter.cpp : Defines the entry point for the application.
//
#include <vector>
#include "Program.h"
#include "PubSubServer.h"
#include "CommandProcessor.h"
#include "LoadBalancer.h"
#include "CommandEmitter.h"

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
	std::cout << "Hello MOClientServer." << std::endl;

  if (argc < 2)
  {
    std::cerr << "Usage: MOClientServer <num_of_clients>" << std::endl;
    return 1;
  }
  int numOfClients = atoi(argv[1]);


  PubSubServerPtr pubSubServer(PubSubServer::getInstance());
  LoadBalancerPtr server(new LoadBalancer());

  std::vector<CommandEmitterPtr> clients;

  for (int i = 0; i < numOfClients; i++)
  {
    clients.push_back(
    CommandEmitterPtr(
        new CommandEmitter("client_" + std::to_string(i))
      )
    );
  }

  std::cout << "enter to exit" << std::endl;

  bool requestedToStop = false;
  std::thread t([&]()
    {
      while (!requestedToStop)
      {
        std::this_thread::sleep_for(1000ms);
         std::cout 
           << " queue:" << MOStat::publishedQueue_
           << " sent:" << MOStat::sent_
           << " received:" << MOStat::received_
           << " procs:" << MOStat::servers_
           << " procQueue:" << MOStat::procQueue_
           
           << std::endl;
      }
    }

  );
  std::cin.get();
  requestedToStop = true;
  t.join();

  for (auto it = clients.begin(); it != clients.end(); it++)
  {
    (*it)->Stop();
  }

  server->Stop();
  pubSubServer->Stop();
	return 0;
}
