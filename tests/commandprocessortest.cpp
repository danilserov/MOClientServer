#include <gtest/gtest.h>

#include "server.h"
#include "client.h"
#include "command.h"

TEST(motestsSuite, ServerCase) {
  ServerPtr server = Server::getInstance();
  ClientPtr client(
    new Client(1)
  );

  CommandPtr command(new Command());
  command->commandType = Command::TODO_COS;
  command->payload_ = 0;

  auto replay = client->ExecuteSync(command);
  ASSERT_FALSE(replay == nullptr);
  EXPECT_TRUE(replay->payload_ > 0.99 && replay->payload_ < 1.01);
}

TEST(motestsSuite, CommandProcessorCase) {

  CommandProcessorDecorator server;

  CommandPtr command(new Command());
  command->commandType = Command::TODO_SIN;

  auto replay = server.ProcessCommand(command);
  ASSERT_FALSE(replay == nullptr);
  EXPECT_EQ(replay->GetCommandId(), command->GetCommandId());
  EXPECT_EQ(replay->timestamp_, command->timestamp_);
}

