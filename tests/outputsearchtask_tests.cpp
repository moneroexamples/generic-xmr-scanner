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
          .WillRepeatedly(Return(1e10)); \
  \
  EXPECT_CALL(mcore, get_current_blockchain_height()) \
          .WillRepeatedly(Return(100));\
  \
  vector blocks {block{}};\
  blocks[0].tx_hashes.push_back(jtx->tx_hash);\
  \
  EXPECT_CALL(mcore, get_blocks_range(_, _)) \
          .WillRepeatedly(\
              Invoke([&blocks](auto h1, auto h2)\
              {\
                  if (h1 - 1 == h2)\
                    return vector<block>();\
                  return blocks; \
              }));\
  \
  vector txs {jtx->tx}; \
  \
  EXPECT_CALL(mcore, get_transactions(_, _, _)) \
        .WillRepeatedly(DoAll(SetArgReferee<1>(txs), \
  				        Return(true)));

namespace
{

using namespace xmreg;
using namespace std;

using ::testing::AtLeast;

template < class T >
inline std::ostream& operator << (std::ostream& os, const std::vector<T>& v) 
{
    os << "[";
    for (auto ii = v.begin(); ii != v.end(); ++ii)
    {
        os << " " << *ii;
    }
    os << " ]";

    return os;
}

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

auto get_outputs_from_json(nl::json const& results) 
{
    vector<JsonTx::output> outputs;

    if (!results.is_array())
    {
        return outputs;
    }

    for (auto const& result: results)
    {
        if (!result.contains("data") 
                || !result["data"].contains("outputs"))
        {
            continue;
        }

        for (auto const& output: result["data"]["outputs"])
        {
            outputs.push_back(
                    JsonTx::output {
                        output["index"],
                        {}, 
                        std::stoull(output["amount"].get<string>()) 
                    });

            hex_to_pod(output["public_key"], 
                       outputs.back().pub_key);
        }
    }

    return outputs;
}

class DifferentJsonTxs :
        public ::testing::TestWithParam<string>
{

};

class OutputScannerTest : public DifferentJsonTxs
{};

INSTANTIATE_TEST_CASE_P(
    DifferentJsonTxs, OutputScannerTest,
    ::testing::Values(
        "ddff95211b53c194a16c2b8f37ae44b643b8bd46b4cb402af961ecabeb8417b2"s,
        "f3c84fe925292ec5b4dc383d306d934214f4819611566051bca904d1cf4efceb"s,
        "d7dcb2daa64b5718dad71778112d48ad62f4d5f54337037c420cb76efdd8a21c"s,
        "61f756a299efd17442eed5437fa03cbda6b01f341907845f8880bf30319fa01c"s,
        "ae8f3ad29a40e02dff6a3267c769f08c0af3dc8858683c90ce3ef90212cb7e4b"s,
        "140807b970e52b7c633d7ca0ba5be603922aa7a2a1213bdd16d3c1a531402bf6"s,
        "a7a4e3bdb305b97c43034440b0bc5125c23b24d0730189261151c0aa3f2a05fc"s,
        "c06df274acc273fbce0666b2c8846ac6925a1931fb61e3020b7cc5410d4646b1"s,
        "d89f32f1434b6a668cbbc5c55cb1c0c64e41fccb89f6b1eef210fefdacbdd89f"s,
        "bd461b938c3c8c8e4d9909852221d5c37350ade05e99ef836d6ccb628f6a5a0e"s,
        "f81ecd0381c0b89f23cffe86a799e924af7b5843c663e8c07db98a14e913585e"s,
        "386ac4fbf7d3d2ab6fd4f2d9c2e97d00527ca2867e33cd7aedb1fd05a4b791ec"s,
        "e658966b256ca30c85848751ff986e3ba7c7cfdadeb46ee1a845a042b3da90db"s
        ));

struct MockTask : public ISearchObserver
{
    MOCK_METHOD1(new_results, void(ISearchTask*));
};

//TEST(OUTPUT_SEARCH, IdentifyOutputs)
TEST_P(OutputScannerTest, IdentifyOutputs)
{
    //auto jtx = construct_jsontx
         //("d7dcb2daa64b5718dad71778112d48ad62f4d5f54337037c420cb76efdd8a21c");
    
    string tx_hash_str = GetParam();
    auto jtx = construct_jsontx(tx_hash_str);
    
    //auto jtx = construct_jsontx
         //("ddff95211b53c194a16c2b8f37ae44b643b8bd46b4cb402af961ecabeb8417b2");

    ASSERT_TRUE(jtx);

    MockMicroCore mcore;
    
    ADD_MOCKS(mcore);

    MockTask mocktask;

    //EXPECT_CALL(mocktask, new_results(_))
              //.Times(AtLeast(1));

    for (auto const& recipient: jtx->recipients)
    {

        if (!recipient.is_subaddress)
            continue;

        //cout << recipient.address_str() << ' ' 
             //<< recipient.is_subaddress << '\n';

        auto task = OutputSearchTask::create(
               recipient.get_addr_viewkey_as_json());

        ASSERT_TRUE(bool {task});

        task->subscribe(&mocktask);
    
        task->set_microcore(&mcore);

        (*task)();

        auto [current_progress, result] = task->get_results();

        auto outputs_expected = recipient.outputs;

        //cout << result << endl;

        auto outputs_found = get_outputs_from_json(result); 
        
        //cout << "found:\n" << outputs_found << '\n';
        //cout << "expected:\n" << outputs_expected << '\n';

        ASSERT_EQ(outputs_found.size(), outputs_expected.size());

        for (size_t i = 0; i < outputs_found.size(); ++i)
        {
            EXPECT_EQ(outputs_found[i], outputs_expected[i]);
        }
    }
}



}

