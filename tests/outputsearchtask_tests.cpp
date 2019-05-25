#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../src/searchtasks/OutputSearchTask.h"
#include "xmregcore/tests/mocks.h"
#include "xmregcore/tests/JsonTx.h"

#define ADD_MOCKS(mcore) \
  EXPECT_CALL(mcore, get_output_tx_and_index(_, _, _)) \
          .WillRepeatedly( \
              Invoke(&*jtx, &JsonTx::get_output_tx_and_index)); \
  \
  EXPECT_CALL(mcore, get_tx(_, _)) \
          .WillRepeatedly( \
              Invoke(&*jtx, &JsonTx::get_tx)); \
  \
  EXPECT_CALL(mcore, get_output_key(_, _, _)) \
          .WillRepeatedly( \
              Invoke(&*jtx, &JsonTx::get_output_key));\
  \
  EXPECT_CALL(mcore, get_num_outputs(_)) \
          .WillRepeatedly(Return(1e10));

namespace
{

using namespace xmreg;
using namespace std;

class ACCOUNT_FIXTURE : public ::testing::Test
{
protected:

ACCOUNT_FIXTURE()
{
	acc = make_account(address, viewkey);
}

string address = "44AFFq5kSiGBoZ4NMDwYtN18obc8AemS33DBLWs3H7otXft3XjrpDtQGv7SqSsaBYBb98uNbr2VBBEt7f2wfn3RVGQBEP3A";
string viewkey = "f359631075708155cc3d92a32b75a7d02a5dcf27756707b47a2b31b21c389501";
unique_ptr<Account> acc; 
};


TEST(OUTPUT_SEARCH_TASK, AccIsNull)
{
    EXPECT_DEATH({OutputSearchTask task {nullptr};}, "");
}

TEST_F(ACCOUNT_FIXTURE, ConstructWithAcc)
{
	OutputSearchTask task {std::move(acc)};
	EXPECT_EQ(task.key(), address + viewkey + "720"s + "1"s);
}

TEST_F(ACCOUNT_FIXTURE, CreateFactoryEmptyJson)
{
    nl::json in_data; 

    auto task = OutputSearchTask::create(in_data);

    EXPECT_FALSE(bool {task});
}

TEST_F(ACCOUNT_FIXTURE, CreateFactoryUsingJson)
{
    nl::json in_data {
        {"address", address},
        {"viewkey", viewkey},
    }; 

    {
        auto task = OutputSearchTask::create(in_data);
        EXPECT_TRUE(bool {task});
    }

    {
        auto in_data2 = nl::json{{"addr", address}};
        auto task = OutputSearchTask::create(in_data2);
        EXPECT_FALSE(bool {task});
    }
    
    {
        auto in_data2 = in_data;

        in_data2["timespan"] = "2";

        auto task = OutputSearchTask::create(in_data2);

        EXPECT_EQ(task->blocks_no(), 7*720);
    }

    {
        auto in_data2 = in_data;

        in_data2["coinbase"] = false;

        auto task = OutputSearchTask::create(in_data2);

        EXPECT_EQ(task->skip_coinbase(), false);
    }
}

TEST(OUTPUT_SEARCH, ForPrimaryAccount)
{

     MockMicroCore mcore;


    
}



}

