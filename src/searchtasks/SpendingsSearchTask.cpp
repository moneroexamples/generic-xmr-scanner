#include "SpendingsSearchTask.h"


namespace xmreg
{


std::string
SpendingsSearchTask::key() const
{
    return "SpendingsSearchTask"
        + m_address + m_viewkey
        + std::to_string(m_blocks_no)
        + std::to_string(m_skip_coinbase);
}

bool
SpendingsSearchTask::process_tx(
        transaction const& tx,
        block const& blk, uint64_t blk_no)
{
    auto identifier = make_identifier(tx,
                        make_unique<Output>(
                            m_acc.get()),
                        make_unique<GuessInput>(
                            m_acc.get(),
                            const_cast<MicroCore*>(m_core)));
    identifier.identify();

    auto const& outputs_found 
        = identifier.get<Output>()->get();

    auto const& inputs_found
        = identifier.get<GuessInput>()->get();

    if (!inputs_found.empty())
    {
        auto possible_total_spent 
            = xmreg::calc_total_xmr(inputs_found)
              - xmreg::calc_total_xmr(outputs_found)
              - get_tx_fee(tx);

        auto msg = jb().success(
                    {
                      {"block", blk_no},
                      {"timestamp", blk.timestamp},
                      {"tx", tx},
                      {"inputs", inputs_found},
                      {"possible_spending", possible_total_spent}
                    });

        cout << msg.dump() << endl;

        results.push_back(std::move(msg));

        notify(this);
    }

    return true;
}

}
