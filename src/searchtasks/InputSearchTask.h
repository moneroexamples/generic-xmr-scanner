#pragma once

#include "OutputSearchTask.h"

namespace xmreg
{

using namespace drogon;
using namespace std;


void
to_json(nl::json& j, Input::info const& info);

class InputSearchTask:  public OutputSearchTask
{

public:

using OutputSearchTask::OutputSearchTask;

virtual std::string
key() const override;

virtual bool
process_tx(transaction const& tx,
           block const& blk,
           uint64_t blk_no) override;

};


}
