#include <gtest/gtest.h>

#include "server.h"
#include "client.h"
#include "command.h"

TEST(motestsSuite, ServerCase) {
  ServerPtr server = Server::getInstance();
  ClientPtr client(
    new Client(1)
  );

  CommandPtr command(new Command(client->GetClientId(), true));
  command->commandType = Command::TODO_COS;
  command->SetPayload(0);

  auto replay = client->ExecuteSync(command);
  ASSERT_FALSE(replay == nullptr);
  EXPECT_TRUE(replay->GetPayload() > 0.99 && replay->GetPayload() < 1.01);
}

TEST(motestsSuite, CommandProcessorCase) {

  CommandProcessorDecorator server;

  CommandPtr command(new Command(-1, true));
  command->commandType = Command::TODO_SIN;

  auto replay = server.ProcessCommand(command);
  ASSERT_FALSE(replay == nullptr);
  EXPECT_EQ(replay->GetCommandId(), command->GetCommandId());
  EXPECT_EQ(replay->GetTimeStamp(), command->GetTimeStamp());
}

