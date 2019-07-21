#include "OutputSearchTask.h"

#include "../JsonBuilder.h"

namespace xmreg
{

void
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


OutputSearchTask::OutputSearchTask(unique_ptr<Account> _acc,
                size_t _no_of_blocks,
                bool _skip_coinbase,
                uint64_t _blocks_lookahead)
    : SearchTask(_no_of_blocks, 
                 _skip_coinbase, 
                 _blocks_lookahead),
      m_acc {std::move(_acc)}
{
    assert(m_acc);
    m_address = m_acc->ai2str();
    m_viewkey = m_acc->vk2str();
}

std::string
OutputSearchTask::key() const
{
    return "OutputSearchTask"
        + m_address + m_viewkey
        + std::to_string(m_blocks_no)
        + std::to_string(m_skip_coinbase);
}

bool
OutputSearchTask::process_tx(
        transaction const& tx,
        block const& blk, uint64_t blk_no)
{
    auto identifier = make_identifier(tx,
                        make_unique<Output>(
                            m_acc.get()));
    identifier.identify();

    auto const& outputs_found 
        = identifier.get<0>()->get();
        
    if (!outputs_found.empty())
    {
        auto tx_hash = pod_to_hex(
                cryptonote::get_transaction_hash(tx)); 

        auto msg = jb().success(
                    {
                      {"block", blk_no},
                      {"timestamp", blk.timestamp},
                      {"tx", tx_hash},
                      {"outputs", outputs_found}
                    });

        results.push_back(std::move(msg));

        notify(this);
    }

    return true;
}

}
