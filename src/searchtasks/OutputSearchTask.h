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

    string subaddr_idx {"N/A"};


    if (info.has_subaddress_index())
    {
        subaddr_idx = std::to_string(info.subaddr_idx.major)
                      + "/"
                      + std::to_string(info.subaddr_idx.minor);
    }

    j = nl::json {
        {"public_key", pod_to_hex(info.pub_key)},
        {"amount", std::to_string(info.amount)},
        {"index", info.idx_in_tx},
        {"subaddr_idx", subaddr_idx}
        
    };
}

class OutputSearchTask:  public ISearchTask 
{

public:


OutputSearchTask(unique_ptr<Account> _acc,
                size_t _no_of_blocks = 720,
                bool _skip_coinbase = true)
    : m_acc {std::move(_acc)},
      m_blocks_no {_no_of_blocks},
      m_skip_coinbase {_skip_coinbase} 
{
    assert(m_acc);
    m_address = m_acc->ai2str();
    m_viewkey = m_acc->vk2str();
}

std::string key() const override 
{
    //return std::to_string(t_no) + "_" + m_address + m_viewkey;
    return m_address + m_viewkey 
        + std::to_string(m_blocks_no)
        + std::to_string(m_skip_coinbase);
}


void operator()() override
{
    auto last_block_height = current_height(); 

    uint64_t blocks_lookahead = 10;

    auto searched_blk_no = last_block_height - m_blocks_no;

    std::thread::id my_thread = std::this_thread::get_id(); 

    {
        std::ostringstream buffer;

        buffer << key().substr(0, 6) 
               << " is on thread " 
               << my_thread << '\n';

        std::cout << buffer.str() << std::flush;
    }

    auto time1 = chrono::system_clock::now();

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
        
        //cout << "blocks no: " << blocks.size() << '\n';

        if (blocks.empty())
            break;
        
        auto time2 = chrono::system_clock::now();

        if (time2 - time1 > 2s || h2 == last_block_height)
        {
            // update search progress status
            // every number of seconds
            
            ostringstream os;

            os  << key().substr(0, 6) << " " 
                  << blocks.size() << " blocks from "
                  << h1 << " to " << h2
                  << " out of " << last_block_height 
                  << " blocks\n";

            //cout << os.str();

            //std::lock_guard lk {m_mtx_results};

            current_progress = jb().success(
                        {{"progress", 
                         {
                           {"current_block", h1},
                           {"blockchain_height", 
                                last_block_height}
                         }
                        }});
            
            notify(this);

            time1 = chrono::system_clock::now();
       }

        //for (auto const& blk: blocks)
        for (uint64_t i {0}; i < blocks.size(); ++i)
        {
            auto const& blk = blocks[i];

            vector<transaction> txs;

            //cout << "i: " << i << '\n';

            txs.reserve(blk.tx_hashes.size() 
                        + 1 /*for miner tx*/);

            vector<crypto::hash> missed_txs;

            m_core->get_transactions(blk.tx_hashes, 
                    txs, missed_txs);

            (void) missed_txs;

            //cout << "txs no: " << txs.size() << '\n';

            if (!m_skip_coinbase)
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
                                        m_acc.get()));
                identifier.identify();

                auto const& outputs_found 
                    = identifier.get<0>()->get();
                    
                //cout << "outputs_found: " << outputs_found.size() <<'\n';

                if (!outputs_found.empty())
                {
                    //cout << "outputs_found: " << outputs_found <<'\n';

                    auto tx_hash = pod_to_hex(
                            cryptonote::get_transaction_hash(tx)); 

                    auto msg = jb().success(
                                {
                                  {"block", h1 + i},
                                  {"timestamp", blk.timestamp},
                                  {"tx", tx_hash},
                                  {"outputs", outputs_found}
                                });

                    //std::lock_guard lk {m_mtx_results};

                    results.push_back(std::move(msg));
        
                    notify(this);
                }
            }
        }
        
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
 size_t timespan {1};
 bool skip_coinbase {true};

 try 
 {
      address = in_data.at("address"); 
      viewkey = in_data.at("viewkey");
      
      if (in_data.contains("timespan")
            && in_data["timespan"].is_string()) 
      {
        timespan = std::atoi(in_data["timespan"]
                           .get<string>().c_str());
      }

      uint64_t no_of_past_blocks {720}; // about 1 day

      switch(timespan)
      {
          case 1: {no_of_past_blocks = 720; break;}
          case 2: {no_of_past_blocks = 7*720; break;}
          case 3: {no_of_past_blocks = 30*720; break;}
      }

      if (in_data.contains("coinbase") && 
              in_data["coinbase"].is_boolean())
      {
          skip_coinbase = in_data["coinbase"];
      }

      auto acc = make_account(address, viewkey);

      if (!acc)
      {
          LOG_DEBUG << "Failed to parse address/viewkey";
          return nullptr;
      }

      unique_ptr<OutputSearchTask> task;

      if (acc->type() == Account::PRIMARY)
      {
          auto pacc = make_primaryaccount(std::move(acc)); 

          task = make_unique<OutputSearchTask>(
                   std::move(pacc), no_of_past_blocks,
                   skip_coinbase);
      }
      else
      {
          task = make_unique<OutputSearchTask>(
                   std::move(acc), no_of_past_blocks,
                   skip_coinbase);
      }
 
      return task;
 }
 catch (std::exception const& e)
 {
     LOG_DEBUG << e.what();
 }

 return nullptr;
}

bool skip_coinbase() const
{
    return m_skip_coinbase;
}

size_t blocks_no() const
{
    return m_blocks_no;
}

protected:

unique_ptr<Account> m_acc; 
size_t m_blocks_no {720};
bool m_skip_coinbase {true};

string m_address;
string m_viewkey;
};


}
