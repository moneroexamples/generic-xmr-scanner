#pragma once

#include "InputSearchTask.h"

namespace xmreg
{

using namespace drogon;
using namespace std;


class SpendingsSearchTask:  public InputSearchTask
{

public:

using InputSearchTask::InputSearchTask;

virtual std::string
key() const override;

virtual bool
process_tx(transaction const& tx,
           block const& blk,
           uint64_t blk_no) override;

};


}
