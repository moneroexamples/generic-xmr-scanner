
#pragma once

#include "ISearchTask.h"

namespace xmreg
{

using namespace std;

class SearchTask: public ISearchTask
{
public:

SearchTask(
        size_t _no_of_blocks = 720,
        bool _skip_coinbase = true,
        uint64_t _blocks_lookahead = 10);

void operator()() override;

virtual bool
process_tx(transaction const& tx,
           block const& blk, 
           uint64_t blk_no) = 0;

bool skip_coinbase() const;
size_t blocks_no() const;

protected:


string 
txhash2str(transaction const& tx) const;


size_t m_blocks_no {720};
bool m_skip_coinbase {true};
uint64_t m_blocks_lookahead {10};
};

}
