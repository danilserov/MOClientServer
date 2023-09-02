// CommandEmitter.cpp : Defines the entry point for the application.
//

#include "Program.h"
#include "PubSubServer.h"
#include "CommandProcessor.h"

int main(int argc, char* argv[])
{
	std::cout << "Hello CommandEmitter." << std::endl;

  if (argc < 2)
  {
    std::cerr << "Usage: CommandEmitter <num_of_clients>" << std::endl;
    return 1;
  }

	return 0;
}
