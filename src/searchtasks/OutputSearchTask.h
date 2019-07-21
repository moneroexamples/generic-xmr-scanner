#pragma once

#include "SearchTask.h"
#include "src/Account.h"
#include "../JsonBuilder.h"

#include "src/UniversalIdentifier.hpp"

#include "drogon/WebSocketController.h"

namespace xmreg
{

using namespace drogon;
using namespace std;

void
to_json(nl::json& j, Output::info const& info);

class OutputSearchTask:  public SearchTask 
{

public:

OutputSearchTask(unique_ptr<Account> _acc,
                size_t _no_of_blocks = 720,
                bool _skip_coinbase = true,
                uint64_t _blocks_lookahead = 10);

virtual std::string
key() const override;

virtual bool
process_tx(transaction const& tx,
           block const& blk,
           uint64_t blk_no) override;

protected:

unique_ptr<Account> m_acc; 

string m_address;
string m_viewkey;
};


}
