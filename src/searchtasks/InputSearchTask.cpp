#include "InputSearchTask.h"


namespace xmreg
{

void
to_json(nl::json& j, Input::info const& info)
{
    string subaddr_idx {"N/A"};

    j = nl::json {
        {"key_image", pod_to_hex(info.key_img)},
        {"public_key", pod_to_hex(info.out_pub_key)},
        {"amount", info.amount}
    };
}


std::string
InputSearchTask::key() const
{
    return "InputSearchTask"
        + m_address + m_viewkey
        + std::to_string(m_blocks_no)
        + std::to_string(m_skip_coinbase);
}

bool
InputSearchTask::process_tx(
        transaction const& tx,
        block const& blk, uint64_t blk_no)
{
    auto identifier = make_identifier(tx,
                        make_unique<GuessInput>(
                            m_acc.get(),
                            const_cast<MicroCore*>(m_core)));
    identifier.identify();

    auto const& inputs_found
        = identifier.get<0>()->get();
        
    if (!inputs_found.empty())
    {
        auto tx_hash = pod_to_hex(
                cryptonote::get_transaction_hash(tx)); 

        auto msg = jb().success(
                    {
                      {"block", blk_no},
                      {"timestamp", blk.timestamp},
                      {"tx", tx_hash},
                      {"inputs", inputs_found}
                    });

        cout << msg.dump() << endl;

        results.push_back(std::move(msg));

        notify(this);
    }

    return true;
}

}
