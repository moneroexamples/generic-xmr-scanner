#include "InputSearchLiteTask.h"


namespace xmreg
{

bool
InputSearchLiteTask::process_tx(
        transaction const& tx,
        block const& blk, uint64_t blk_no)
{
    // first identify outputs so we can 
    // populate known_ouputs
    auto identifier1 = make_identifier(tx,
                        make_unique<Output>(
                            m_acc.get()));

    identifier1.identify();

    auto const& outputs_found 
        = identifier1.get<0>()->get();
        
    if (outputs_found.empty()
            && known_outputs.empty())
    {
        // no first outputs in this tx, 
        // so there is nothing to po
        return true;
    }

    // add found outputs, if any, 
    // into known_outputs
    for (auto const& output: outputs_found)
    {
        known_outputs.insert({
                output.pub_key, 
                output.amount});
    }


    // search for inputs based on known_outputs
    auto identifier2 = make_identifier(tx,
                        make_unique<Input>(
                            m_acc.get(),
                            &known_outputs,
                            const_cast<MicroCore*>(m_core)));
    identifier2.identify();

    auto const& inputs_found
        = identifier2.get<0>()->get();
        
    if (!inputs_found.empty())
    {
        auto msg = jb().success(
                    {
                      {"block", blk_no},
                      {"timestamp", blk.timestamp},
                      {"tx", txhash2str(tx)},
                      {"inputs", inputs_found}
                    });

        cout << msg.dump() << endl;

        results.push_back(std::move(msg));

        notify(this);
    }

    return true;
}

}
