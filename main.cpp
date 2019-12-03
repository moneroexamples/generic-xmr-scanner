#include "drogon/drogon.h"

#include "src/monero_headers.h"

#include "src/SearchTaskManager.h"
#include "src/controllers/SearchCtrl.h"
#include "src/controllers/InfoCtrl.h"
#include "src/config/DefaultAddresses.h"
#include "src/utils.hpp"
#include "ext/xmregcore/src/tools.h"

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
auto config_path = any_cast<string>(
                options["config_path"]);
auto default_addresses_path = any_cast<string>(
                options["default_addresses_path"]);
auto port = any_cast<size_t>(options["port"]);
auto fiberpool_threads_no = any_cast<size_t>(
            options["fiberpool_threads"]);

// setup monero's own logger
mlog_configure(mlog_get_default_log_path(""), true);
mlog_set_log("1");

if (!filesystem::exists(blockchain_path))
{
    cerr << blockchain_path 
         << " does not exist!\n";
    return EXIT_FAILURE;
}

LOG_INFO << "Blockchain path: " << blockchain_path;

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

// reads default addresses to be shown by 
// the frontend for the confing file provided
if (!filesystem::exists(default_addresses_path))
{
    cerr << default_addresses_path 
         << "does not exist!\n";
    return EXIT_FAILURE;
}

if (!filesystem::exists(config_path))
{
    cerr << config_path << "does not exist!\n";
    return EXIT_FAILURE;
}

config_path = xmreg::remove_trailing_path_separator(
        config_path);

LOG_INFO << "Using default addresses from "
         << filesystem::absolute(default_addresses_path);

xmreg::DefaultAddresses default_addresses {
           default_addresses_path};

LOG_INFO << "Using default config from "
         << filesystem::absolute(config_path);

xmreg::DefaultConfig default_config {config_path};

// check if ssl is enabled
auto ssl = default_config.ssl();

filesystem::path ssl_key;
filesystem::path ssl_crt;

if (ssl.enabled)
{
    // check if given server key and crt files exist
    
    ssl_key = xmreg::expand(ssl.key);
    ssl_crt = xmreg::expand(ssl.crt);

    if (!std::filesystem::exists(ssl_key))
    {
        cerr << ssl_key << " ssl-key does not exist!\n";
        return EXIT_FAILURE;
    }
    
    if (!std::filesystem::exists(ssl_crt))
    {
        cerr << ssl_crt << " ssl-crt does not exist!\n";
        return EXIT_FAILURE;
    }

    LOG_INFO << "ssl key: " << ssl_key;
    LOG_INFO << "ssl crt: " << ssl_crt;
}


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
auto task_manager = SearchTaskManager(
        &mcore, &fiber_pool, &default_config);

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
// frontend as well as the list of default addresses.
auto info_ctrl = make_shared<InfoCtrl>(
        InfoCtrl(&task_manager, &default_addresses));
app().registerController(info_ctrl);

LOG_INFO << "InfoCtrl registered";

LOG_INFO << "Listening at 127.0.0.1:" << port;

app().setThreadNum(1);
app().setIdleConnectionTimeout(1h);

if (ssl.enabled)
{
    app().addListener("0.0.0.0", port, true,
                      ssl_crt, ssl_key);
    
    LOG_INFO << "SSL enabled.";
}
else
{
    app().addListener("0.0.0.0", port);
    LOG_INFO << "SSL not enabled.";
}

app().run();

return EXIT_SUCCESS;
}
