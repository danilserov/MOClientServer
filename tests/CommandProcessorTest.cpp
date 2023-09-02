#include <gtest/gtest.h>

#include "PubSubServer.h"
#include "LoadBalancer.h"
#include "CommandEmitter.h"
#include "Command.h"

TEST(MyTestSuite, LoadBalancerCase) {
  LoadBalancerPtr server(new LoadBalancer());
  CommandEmitterPtr client(
    new CommandEmitter("test_sync_client")
  );

  long command_id = 1234567;
  CommandPtr command(new Command(command_id));
  command->topic_ = PubSubServer::TOPIC_COMMAND;

  auto replay = client->ExecuteSync(command);
  ASSERT_FALSE(replay == nullptr);
  EXPECT_EQ(replay->commandId_, command_id);
}

