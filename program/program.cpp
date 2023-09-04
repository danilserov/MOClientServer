#include <vector>

#include "program.h"
#include "pubsubserver.h"
#include "commandprocessor.h"
#include "loadbalancer.h"
#include "commandemitter.h"
#include "mostat.h"

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
	std::cout << "Hello MOClientServer." << std::endl;

  int numOfClients = 10;

  if (argc < 2)
  {
    std::cerr << "Usage: MOClientServer <num_of_clients> <num_of_commands_per_client" << std::endl;
    std::cout << "will create: " << numOfClients << " clients by default" << std::endl;
  }
  else
  {
    numOfClients = atoi(argv[1]);
  }

  int numOfMessages = 1000000;

  if (argc > 2)
  {
    numOfMessages = atoi(argv[2]);
  }


  PubSubServerPtr pubSubServer(PubSubServer::getInstance());
  LoadBalancerPtr server(new LoadBalancer());

  std::vector<CommandEmitterPtr> clients;

  for (int i = 0; i < numOfClients; i++)
  {
    clients.push_back(
    CommandEmitterPtr(
        new CommandEmitter("client_" + std::to_string(i), numOfMessages)
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
           << "PubQueue:" << MOStat::publishedQueue_ << "\t"
           << "Sent:" << MOStat::sent_ << "\t"
           << "Received:" << MOStat::received_ << "\t"
           << "Procs:" << MOStat::servers_ << "\t"
           << "ProcQueue:" << MOStat::procQueue_ << "\t"

           << "RoundTripMax["
           << "Sync:" << MOStat::maxSyncTime_ << "\t"
           << "Async:" << MOStat::maxAsyncTime_ << "]\t"
           
           
           << std::endl;
         MOStat::maxSyncTime_.store(0);
         MOStat::maxAsyncTime_.store(0);
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
