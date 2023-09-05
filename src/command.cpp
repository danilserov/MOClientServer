#include "command.h"


std::atomic_int Command::idGenerator_(1);

long long Command::GetTimeStamp()
{
  auto current_time = std::chrono::system_clock::now();
  std::chrono::milliseconds timestamp =
    std::chrono::duration_cast<std::chrono::milliseconds>(
      current_time.time_since_epoch()
    );
  return timestamp.count();
}

Command::Command() :
  clientId_(-1),
  highPrior_(false)
{
  commandId_ = idGenerator_++;
  timestamp_ = GetTimeStamp();
}

int Command::GetCommandId()
{
  return commandId_;
}

CommandPtr Command::CreateReplay(const double payload)
{
  CommandPtr replay(new Command());
  replay->timestamp_ = timestamp_;
  replay->payload_ = payload;
  replay->clientId_ = clientId_;
  replay->commandId_ = commandId_;
  return replay;
}