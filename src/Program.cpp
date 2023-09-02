// CommandEmitter.cpp : Defines the entry point for the application.
//
#include <vector>
#include "Program.h"
#include "PubSubServer.h"
#include "CommandProcessor.h"
#include "LoadBalancer.h"
#include "CommandEmitter.h"

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
  std::cin.get();
	return 0;
}
