#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include "InputSearchTask.h"
#include "InputSearchLiteTask.h"

#include "../config/DefaultConfig.h"


namespace xmreg
{

using namespace drogon;
using namespace std;

namespace nl = nlohmann;

/*
* Factory method to construct the task's
* object
*/
template<typename SEARCH_TASK_T>
unique_ptr<SEARCH_TASK_T>
make_task(nl::json const& in_data,
          DefaultConfig* config = nullptr)
{
 string address;
 string viewkey;
 uint64_t no_of_past_blocks {720}; // about 1 day
 bool skip_coinbase {true};

 try
 {
      address = in_data.at("address");
      viewkey = in_data.at("viewkey");

      if (in_data.contains("timespan")
            && in_data["timespan"].is_string())
      {
        no_of_past_blocks = std::atoll(in_data["timespan"]
                           .get<string>().c_str());
      }

      // check if no_of_past_blocks provided by
      // the client is allowed

      if (config &&
              !ScanningFrom::allowed_no_of_blocks(
                  config->scannig_from(),
                  no_of_past_blocks))
      {
          LOG_DEBUG << "no_of_past_blocks "
                    << no_of_past_blocks
                    << " not allowed";

          return nullptr;
      }

      if (in_data.contains("coinbase") &&
             in_data["coinbase"].is_boolean())
      {
          skip_coinbase = in_data["coinbase"];
      }

      auto acc = make_account(address, viewkey);

      if (!acc)
      {
          LOG_DEBUG << "Failed to parse address/viewkey";
          return nullptr;
      }

      uint64_t blocks_lookahead
          = config ? config->blocks_lookahead : 10;

      unique_ptr<SEARCH_TASK_T> task;

      if (acc->type() == Account::PRIMARY)
      {
          auto pacc = make_primaryaccount(std::move(acc));

          task = make_unique<SEARCH_TASK_T>(
                   std::move(pacc),
                   no_of_past_blocks,
                   skip_coinbase,
                   blocks_lookahead);
      }
      else
      {
          task = make_unique<SEARCH_TASK_T>(
                   std::move(acc),
                   no_of_past_blocks,
                   skip_coinbase,
                   blocks_lookahead);
      }

      return task;
 }
 catch (std::exception const& e)
 {
     LOG_DEBUG << e.what();
 }

 return nullptr;
}

}


#endif // TASKFACTORY_H
