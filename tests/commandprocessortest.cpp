#include <gtest/gtest.h>

#include "server.h"
#include "slient.h"
#include "command.h"

TEST(motestsSuite, ServerCase) {
  ServerPtr server = Server::getInstance();
  ClientPtr client(
    new Client(1)
  );

  long command_id = 1234567;
  CommandPtr command(new Command(command_id));
  command->commandType = Command::TODO_COS;
  command->payload_ = 0;

  auto replay = client->ExecuteSync(command);
  ASSERT_FALSE(replay == nullptr);
  EXPECT_TRUE(replay->payload_ > 0.99 && replay->payload_ < 1.01);
}

TEST(motestsSuite, CommandProcessorCase) {

  CommandProcessorDecorator server;

  long command_id = 1234567;
  CommandPtr command(new Command(command_id));
  command->commandType = Command::TODO_SIN;

  auto replay = server.ProcessCommand(command);
  ASSERT_FALSE(replay == nullptr);
  EXPECT_EQ(replay->commandId_, command_id);
  EXPECT_EQ(replay->timestamp_, command->timestamp_);
}

