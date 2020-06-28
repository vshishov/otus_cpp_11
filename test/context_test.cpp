#include "commander.h"
#include "executer.h"
#include "context.h"

#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>

using namespace testing;

class Context_Test : public ::testing::Test
{
protected:
  void Run(std::size_t a_nBlockSize, std::string a_strTestValue, std::size_t a_szDelay = 1)
  {      
    auto pContext = std::make_shared<Otus::Context>(a_nBlockSize, ssLogOut, ssMainMetricsOut, ssLogMetricsOut, ssFileMetricsOut);
    pContext->ProccessBuffer(a_strTestValue.c_str(), a_strTestValue.size());
    std::this_thread::sleep_for(std::chrono::seconds(a_szDelay));

    void* context = pContext.get();
    ssContext << context;
  }

protected:
  std::stringstream ssLogOut;

  std::stringstream ssMainMetricsOut;
  std::stringstream ssLogMetricsOut;
  std::stringstream ssFileMetricsOut;

  std::stringstream ssContext;
};

TEST_F(Context_Test, empty_stream) {   
  Run(2, "");

  ASSERT_EQ(ssLogOut.str(), "");
}

TEST_F(Context_Test, not_empty_stream) { 
  Run(3, "cmd1\ncmd2\ncmd3\n");

  ASSERT_EQ(ssLogOut.str(), "bulk: cmd1, cmd2, cmd3\n");
  ASSERT_EQ(ssMainMetricsOut.str(), ssContext.str() + " main: 3 line(s), 1 block(s), 3 command(s)\n");
  ASSERT_EQ(ssLogMetricsOut.str(), ssContext.str() + " log: 1 block(s), 3 command(s)\n");
}

TEST_F(Context_Test, not_complete) {   
  Run(3, "cmd1\ncmd2\n");
  
  ASSERT_EQ(ssLogOut.str(), "");
  ASSERT_EQ(ssMainMetricsOut.str(), ssContext.str() + " main: 2 line(s), 0 block(s), 0 command(s)\n");
  ASSERT_EQ(ssLogMetricsOut.str(), ssContext.str() + " log: 0 block(s), 0 command(s)\n");
}

TEST_F(Context_Test, single_block) { 
  Run(3, "{\ncmd1\ncmd2\ncmd3\ncmd4\ncmd5\n}\n");

  ASSERT_EQ(ssLogOut.str(), "bulk: cmd1, cmd2, cmd3, cmd4, cmd5\n");
  ASSERT_EQ(ssMainMetricsOut.str(), ssContext.str() + " main: 7 line(s), 1 block(s), 5 command(s)\n");
  ASSERT_EQ(ssLogMetricsOut.str(), ssContext.str() + " log: 1 block(s), 5 command(s)\n");
}

TEST_F(Context_Test, single_block_in_default_block) { 
  Run(3, "\ncmd1\ncmd2\n{\ncmd3\ncmd4\ncmd5\ncmd6\ncmd7\n}\n");

  ASSERT_EQ(ssLogOut.str(), "bulk: cmd1, cmd2\nbulk: cmd3, cmd4, cmd5, cmd6, cmd7\n");
  ASSERT_EQ(ssMainMetricsOut.str(), ssContext.str() + " main: 9 line(s), 2 block(s), 7 command(s)\n");
  ASSERT_EQ(ssLogMetricsOut.str(), ssContext.str() + " log: 2 block(s), 7 command(s)\n");
}

TEST_F(Context_Test, multi_block) { 
  Run(3, "cmd1\ncmd2\n{\ncmd1.1\ncmd1.2\ncmd1.3\n{\ncmd2.1\ncmd2.2\ncmd2.3\ncmd2.4\ncmd2.5\n}\ncmd1.4\ncmd1.5\n}\n");

  ASSERT_EQ(ssLogOut.str(), "bulk: cmd1, cmd2\n"
    "bulk: cmd1.1, cmd1.2, cmd1.3, "
    "cmd2.1, cmd2.2, cmd2.3, cmd2.4, cmd2.5, "
    "cmd1.4, cmd1.5\n");
  ASSERT_EQ(ssMainMetricsOut.str(), ssContext.str() + " main: 16 line(s), 2 block(s), 12 command(s)\n");
  ASSERT_EQ(ssLogMetricsOut.str(), ssContext.str() + " log: 2 block(s), 12 command(s)\n");
}

TEST_F(Context_Test, blocks_not_complete) { 
  Run(3, "cmd1\ncmd2\n{\ncmd1.1\ncmd1.2\ncmd1.3\n{\ncmd2.1\ncmd2.2\ncmd2.3\ncmd2.4\ncmd2.5\n}\ncmd1.4\ncmd1.5\n");

  ASSERT_EQ(ssLogOut.str(), "bulk: cmd1, cmd2\n");
  ASSERT_EQ(ssMainMetricsOut.str(), ssContext.str() + " main: 15 line(s), 1 block(s), 2 command(s)\n");
  ASSERT_EQ(ssLogMetricsOut.str(), ssContext.str() + " log: 1 block(s), 2 command(s)\n");
}

TEST_F(Context_Test, not_correct_sequence) { 
  Run(3, "cmd1\ncmd2\n}\n");
  ASSERT_EQ(ssLogOut.str(), "");
}
