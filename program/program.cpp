#include <vector>

#include "program.h"
#include "commandprocessor.h"
#include "server.h"
#include "client.h"
#include "mostat.h"

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
	std::cout << "Hello MOClientServer." << std::endl;

  int numOfClients = 10;

  if (argc < 2)
  {
    std::cerr << "Usage: MOClientServer "
     << "<num_of_clients> <num_of_commands_per_client" << std::endl;
    std::cout << "will create: " << numOfClients << " clients by default" << std::endl;
  }
  else
  {
    numOfClients = atoi(argv[1]);
  }

  int numOfMessages = 10;

  if (argc > 2)
  {
    numOfMessages = atoi(argv[2]);
  }

  std::vector<ClientPtr> clients;

  for (int i = 0; i < numOfClients; i++)
  {
    clients.push_back(ClientPtr(new Client(i)));
  }

  std::unordered_map<int, std::vector<int>> commandIds;

  for (int j = 0; j < numOfMessages; j++)
  {
    for (int i = 0; i < numOfClients; i++)
    {
      commandIds[clients[i]->GetClientId()].push_back(
        clients[i]->CosAsync(i + j)
      );
      /*try
      {
        auto res = clients[i]->Cos(j);
      }
      catch (const std::exception& ex)
      {
        std::cerr << ex.what() << std::endl;
      }*/
    }
  }

  for (auto it = clients.begin(); it != clients.end(); it++)
  {
    auto commands = commandIds[(*it)->GetClientId()];

    for (auto itCommand = commands.begin(); itCommand != commands.end(); itCommand++)
    {
      auto result = (*it)->GetResult(*itCommand);

      std::cout << "result of " << *itCommand  << "=" << result << std::endl;
    }
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

	return 0;
}
