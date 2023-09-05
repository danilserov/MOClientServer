#include "command.h"


std::atomic_int Command::idGenerator_(1);

long long Command::GetCurTimeStamp()
{
  auto current_time = std::chrono::system_clock::now();
  std::chrono::milliseconds timestamp =
    std::chrono::duration_cast<std::chrono::milliseconds>(
      current_time.time_since_epoch()
    );
  return timestamp.count();
}

Command::Command(int clientId, bool highPrior) :
  clientId_(clientId),
  highPrior_(highPrior)
{
  commandId_ = idGenerator_++;
  timestamp_ = GetCurTimeStamp();
}

int Command::GetCommandId()
{
  return commandId_;
}

long long Command::GetTimeStamp()
{
  return timestamp_;
}

int Command::GetClientId()
{
  return clientId_;
}

bool Command::IsHighPrior()
{
  return highPrior_;
}

void Command::SetPayload(double a)
{
  payload_ = a;
}

double Command::GetPayload()
{
  return payload_;
}

CommandPtr Command::CreateReplay(const double payload)
{
  CommandPtr replay(new Command(clientId_, false));
  replay->timestamp_ = timestamp_;
  replay->payload_ = payload;
  replay->clientId_ = clientId_;
  replay->commandId_ = commandId_;
  return replay;
}