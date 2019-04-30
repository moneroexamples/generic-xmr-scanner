#pragma once

#include "ISearchTask.h"
#include "src/Account.h"
#include "../JsonBuilder.h"

#include "src/UniversalIdentifier.hpp"

#include "drogon/WebSocketController.h"

#include <boost/fiber/all.hpp>

#include <atomic>
#include <string>
#include <thread>


namespace xmreg
{

using namespace drogon;
using namespace std;



inline void 
to_json(nl::json& j, Output::info const& info)
{

    j = nl::json {
        {"public_key", pod_to_hex(info.pub_key)},
        {"amount", std::to_string(info.amount)}
    };
}

class OutputSearchTask:  public ISearchTask 
{

public:


OutputSearchTask(unique_ptr<Account> _acc,
                size_t _timespan = 1)
    : m_acc {std::move(_acc)},
      m_timespan {_timespan} 
{
    m_address = m_acc->ai2str();
    m_viewkey = m_acc->vk2str();
}

std::string key() const override 
{
    return std::to_string(t_no) + "_" + m_address + m_viewkey;
}


void operator()() override
{
    assert(m_core);

    auto last_block_height = current_height(); 

    uint64_t blocks_lookahead = 10;

    auto searched_blk_no = last_block_height - 1000;

    auto addr = m_acc->ai();
    auto vk   = m_acc->vk().value();

    std::thread::id my_thread = std::this_thread::get_id(); 

    {
        std::ostringstream buffer;

        buffer << key().substr(0, 6) 
               << " is on thread " 
               << my_thread << '\n';

        std::cout << buffer.str() << std::flush;
    }


    while(!m_should_finish)
    {

        last_block_height = current_height();

        uint64_t h1 = searched_blk_no;
        uint64_t h2 = std::min(h1 + blocks_lookahead - 1, 
                               last_block_height);

        vector<block> blocks;

        try 
        {
            blocks = m_core->get_blocks_range(h1, h2);
        }
        catch (std::exception const& e)
        {
            LOG_WARN << "m_core->get_blocks_range(h1, h2): " 
                      << e.what();
            break;
        }

        if (blocks.empty())
        {
            break;
        }
        
        {
            ostringstream os;

            os  << key().substr(0, 6) << " " 
                  << blocks.size() << " blocks from "
                  << h1 << " to " << h2
                  << " out of " << last_block_height 
                  << " blocks\n";

            cout << os.str();

            auto msg = jb().success(
                        {{"progress", 
                         {
                           {"current_block", h1},
                           {"blockchain_height", 
                                last_block_height}
                         }
                        }});

            results.push_back(std::move(msg));
        }
        
        //continue;

        for (auto const& blk: blocks)
        {
                //boost::this_fiber::sleep_for(0ms);

                //std::thread::id new_thread 
                    //= std::this_thread::get_id();

                 //if (new_thread != my_thread) 
                 //{
                    //my_thread = new_thread;

                    //std::ostringstream buffer;

                    //buffer << key().substr(0, 6) 
                           //<< " switched thread to " 
                           //<< my_thread << '\n';

                    ////std::cout << buffer.str() << std::flush;
                 //}

            vector<transaction> txs;

            txs.reserve(blk.tx_hashes.size() 
                        + 1 /*for miner tx*/);

            vector<crypto::hash> missed_txs;

            m_core->get_transactions(blk.tx_hashes, 
                    txs, missed_txs);

            (void) missed_txs;

            txs.push_back(blk.miner_tx);

            for (auto const& tx: txs)
            {
                //boost::this_fiber::yield();
                boost::this_fiber::sleep_for(0ms);

                std::thread::id new_thread 
                    = std::this_thread::get_id();

                 if (new_thread != my_thread) 
                 {
                    my_thread = new_thread;

                    std::ostringstream buffer;

                    buffer << key().substr(0, 6) 
                           << " switched thread to " 
                           << my_thread << '\n';

                    //std::cout << buffer.str() << std::flush;
                 }

                auto identifier = make_identifier(tx,
                                    make_unique<Output>(
                                        &addr, &vk));
                identifier.identify();

                auto const& outputs_found 
                    = identifier.get<0>()->get();

                if (!outputs_found.empty())
                {
                    auto tx_hash = pod_to_hex(
                            cryptonote::get_transaction_hash(tx)); 

                    auto msg = jb().success(
                                {
                                  {"tx", tx_hash},
                                  {"outputs", outputs_found}
                                });

                    results.push_back(std::move(msg));
                }
            }
        }
        
        notify(this);

        searched_blk_no = h2 + 1;
    }


    {
        std::ostringstream buffer;
        buffer<< key().substr(0, 6) << " finished\n";
        std::cout << buffer.str() << std::flush;
        
        nl::json msg {{"found: ", buffer.str()}};
        results.push_back(std::move(msg));
        notify(this);
    }
}

/*
* Factory method to construct the task's
* object
*/
static unique_ptr<OutputSearchTask> 
create(nl::json const& in_data)
{
 string address;
 string viewkey;
 size_t timespan;

 try 
 {
      address = in_data["address"]; 
      viewkey = in_data["viewkey"];
      timespan = std::atoi(in_data["timespan"]
                           .get<string>().c_str());

      auto acc = account_factory(address, viewkey);

      if (!acc)
      {
          LOG_DEBUG << "Failed to parse address/viewkey";
          return nullptr;
      }
 
      auto task = make_unique<OutputSearchTask>(
             std::move(acc), timespan);

      return task;
 }
 catch (std::exception const& e)
 {
     LOG_DEBUG << e.what();
 }

 return nullptr;
}

protected:

unique_ptr<Account> m_acc; 
size_t m_timespan {1};
string m_address;
string m_viewkey;
};


}
