#pragma once

#include "InputSearchTask.h"

namespace xmreg
{

using namespace drogon;
using namespace std;


class InputSearchLiteTask:  public InputSearchTask
{

public:

using InputSearchTask::InputSearchTask;

virtual bool
process_tx(transaction const& tx,
           block const& blk,
           uint64_t blk_no) override;

protected:

Input::known_outputs_t known_outputs;

};





}
