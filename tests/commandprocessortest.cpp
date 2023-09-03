#include <gtest/gtest.h>

#include "pubsubserver.h"
#include "loadbalancer.h"
#include "commandemitter.h"
#include "command.h"

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
