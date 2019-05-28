#include "drogon/drogon.h"
#include "src/monero_headers.h"

#include "src/SearchTaskManager.h"
#include "src/controllers/SearchCtrl.h"
#include "src/controllers/InfoCtrl.h"
#include "src/utils.hpp"

#include <boost/fiber/all.hpp> 

#include <iostream>

using namespace FiberPool;
using namespace xmreg;

using namespace cryptonote;
using namespace drogon;
using namespace std::literals::chrono_literals;


int
main(int argc, const char *argv[])
{

auto options = process_program_options(argc, argv);

if (any_cast<bool>(options["error"])
        || any_cast<bool>(options["help"]))
    return EXIT_SUCCESS;

auto net_type = any_cast<network_type>(
                options["nettype"]);
auto blockchain_path = any_cast<string>(
                options["blockchain_path"]);
auto port = any_cast<size_t>(options["port"]);
auto fiberpool_threads_no = any_cast<size_t>(
            options["fiberpool_threads"]);

// setup monero's own logger
mlog_configure(mlog_get_default_log_path(""), true);
mlog_set_log("1");

// initialize and create instance of MicroCore
// which is going to provide direct access to monero
// blockchain
auto mcore = MicroCore(blockchain_path, 
                       net_type);

LOG_INFO << "MicroCore initialized";

auto current_height = mcore.get_core()
        .get_current_blockchain_height();

LOG_INFO << "Current blockchain height: " 
         << current_height;

if (fiberpool_threads_no == 0)
{
    fiberpool_threads_no = no_of_defualt_threads();
}

LOG_INFO << "FiberPool will use " << fiberpool_threads_no 
         << " thread workers";

// create FiberPool which uses work_sharing scheduler
// to manage fibers. all tasks submitted to the scanner
// will run as fibers in the pool.
FiberPoolSharing<> fiber_pool(fiberpool_threads_no);

// the manager will keep track of all search tasks submitted 
// to our service. This is main scope variable, so we are just
// going to keep passing NON-OWNING raw pointer to it 
// into services that need to use it. 
// Basically any raw pointer is NON-OWNING!
auto task_manager = SearchTaskManager(&mcore, &fiber_pool);

// start search tasks management loop. It mostly just
// checks periodically for finished tasks and lost
// websockets coonections. 
fiber_pool.submit(&SearchTaskManager::managment_loop, 
                  &task_manager);

LOG_INFO << "SearchTaskManager loop started";

// create websocket controller. this will listen for search
// requrests from clients and submits tasks to task_manager
auto search_ws_ctrl = make_shared<SearchWebSocketCtrl>(
                          &task_manager);
app().registerController(search_ws_ctrl);

LOG_INFO << "SearchWebSocketCtrl registered";

// create regular http controller. It will provide
// initial information, e.g., networy type, to the 
// frontend.
auto info_ctrl = make_shared<InfoCtrl>(InfoCtrl(&task_manager));
app().registerController(info_ctrl);

LOG_INFO << "InfoCtrl registered";

LOG_INFO << "Listening at 127.0.0.1:" << port;

app().setThreadNum(1);
app().setIdleConnectionTimeout(1h);
app().addListener("0.0.0.0", port);

app().run();

return EXIT_SUCCESS;
}
