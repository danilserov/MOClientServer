﻿#include <vector>

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
    std::cout << "Usage: moclientserver "
     << "<num_of_clients>" << std::endl;
    std::cout << "will create: " << numOfClients << " clients by default" << std::endl;
  }
  else
  {
    numOfClients = atoi(argv[1]);
  }

  int numOfAsyncSIN_Messages = 1000;
  int numOfAsyncCOS_Messages = 1000;
  int numOfSyncSIN_Messages = 10;
  int numOfSyncCOS_Messages = 10;

  std::vector<ClientPtr> clients;

  for (int i = 0; i < numOfClients; i++)
  {
    clients.push_back(ClientPtr(new Client(i)));
  }

  std::unordered_map<int, std::vector<int>> commandIds;

  // Send async commands.
  for (int i = 0; i < numOfClients; i++)
  {
    for (int j = 0; j < numOfAsyncCOS_Messages; j++)
    {
      commandIds[clients[i]->GetClientId()].push_back(
        clients[i]->CosAsync(i + j)
      );
    }  

    for (int j = 0; j < numOfAsyncSIN_Messages; j++)
    {
      commandIds[clients[i]->GetClientId()].push_back(
        clients[i]->SinAsync(i + j)
      );
    }
  }

  // Execute Sync Commands.
  for (int i = 0; i < numOfClients; i++)
  {
    for (int j = 0; j < numOfSyncCOS_Messages; j++)
    {
      try
      {
        auto res = clients[i]->Cos(j + i);
        std::cout << "cos(" << j + i << ")=" << res << std::endl;
      }
      catch (const std::exception& ex)
      {
        std::cerr << ex.what() << std::endl;
      }
    }
    for (int j = 0; j < numOfSyncSIN_Messages; j++)
    {
      try
      {
        auto res = clients[i]->Sin(j + i);
        std::cout << "sin(" << j + i << ")=" << res << std::endl;
      }
      catch (const std::exception& ex)
      {
        std::cerr << ex.what() << std::endl;
      }
    }
  }

  // Wait for async commands.
  for (auto it = clients.begin(); it != clients.end(); it++)
  {
    auto commands = commandIds[(*it)->GetClientId()];

    for (auto itCommand = commands.begin(); itCommand != commands.end(); itCommand++)
    {
      auto result = (*it)->GetResult(*itCommand);

      std::cout << "result of command[" << *itCommand  << "]=" << result << std::endl;
    }
  }

  std::cout 
    << "Sent:" << MOStat::sent_ << "\t"
    << "Received:" << MOStat::received_ << "\t"
    << "Procs:" << MOStat::servers_ << "\t"

    << "RoundTripMax["
    << "Sync:" << MOStat::maxSyncTime_ << "\t"
    << "Async:" << MOStat::maxAsyncTime_ << "]\t"
           
           
    << std::endl;
         

  for (auto it = clients.begin(); it != clients.end(); it++)
  {
    (*it)->Stop();
  }

  std::cout << "enter to exit" << std::endl;
  std::cin.get();
	return 0;
}
