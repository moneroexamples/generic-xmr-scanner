
#include "SearchTask.h"
#include "../JsonBuilder.h"
#include "drogon/WebSocketController.h"


namespace xmreg
{

SearchTask::SearchTask(
        size_t _no_of_blocks,
        bool _skip_coinbase,
        uint64_t _blocks_lookahead)
    : m_blocks_no {_no_of_blocks},
      m_skip_coinbase {_skip_coinbase},
      m_blocks_lookahead {_blocks_lookahead}
{
}


void 
SearchTask::operator()() 
{
    auto last_block_height = current_height(); 

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
        uint64_t h2 = std::min(h1 + m_blocks_lookahead - 1, 
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

        //cout << "h1,h2 = " << h1 << ',' << h2 <<  '\n';
        
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
                                last_block_height},
                           {"timestamp", 
                                blocks.front().timestamp}
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

                 this->process_tx(tx, blk, h1 + i);

            } // for (auto const& tx: txs)

        } //for (uint64_t i {0}; i < blocks.size(); ++i)

        searched_blk_no = h2 + 1;

    } // while(!m_should_finish)
    
    results.push_back(jb().success({{"finished", true}}));
    notify(this);
}


bool 
SearchTask::skip_coinbase() const
{
    return m_skip_coinbase;
}

size_t 
SearchTask::blocks_no() const
{
    return m_blocks_no;
}

}
