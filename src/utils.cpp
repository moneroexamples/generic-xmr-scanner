#include "utils.hpp"
#include "src/tools.h"

#include <boost/program_options.hpp>
#include "boost/algorithm/string.hpp"


namespace xmreg
{

namespace po = boost::program_options;

map<string, std::any>
process_program_options(int argc, const char *argv[])
{
map<string, std::any> options;

options["help"] = false;
options["error"] = false;
options["nettype"] = network_type::STAGENET;

auto default_blockchain_path = xmreg::get_default_lmdb_folder(
            any_cast<network_type>(options["nettype"]));

options["blockchain_path"] = default_blockchain_path;

options["default_addresses_path"] 
    = "./config/default_addresses.json"s;

options["config_path"] = "./config/config.json"s;

string blockchain_path_msg {
        "Non-default path to lmdb folder " 
        "containing the blockchain. Default is "
        + default_blockchain_path};

try
{
    po::options_description desc("xmrscanner: "
           "generic monero scanner");

    desc.add_options()
        ("help,h", "Help")
        ("nettype,n", po::value<size_t>()->default_value(
             static_cast<size_t>(
                 any_cast<network_type>(options["nettype"]))),
         "network type: 0-MAINNET, 1-TESTNET, 2-STAGENET")
        ("blockchain-path,b", po::value<string>(),
             blockchain_path_msg.c_str())
        ("da-path,d", po::value<string>()
             ->default_value("./config/default_addresses.json"),
            "Non-default path to default_addresses.json file")        
        ("config-path,c", po::value<string>()
             ->default_value("./config/config.json"),
            "Non-default path to config.json file")        
        ("fiberthreads,f",
             po::value<size_t>()->default_value(0),
         "Number of fiberpool thread workers. "
         "Default is 0 - based on hardware")
        ("port,p", po::value<size_t>()->default_value(8848),
         "Service port");

    po::positional_options_description pos_desc;
    pos_desc.add("hash", -1);

    po::command_line_parser parser{argc, argv};
    parser.options(desc).positional(pos_desc)
        .allow_unregistered();
    po::parsed_options parsed_options = parser.run();


    po::variables_map vm;
    store(parsed_options, vm);
    notify(vm);

    if (vm.count("help"))
    {
        cout << desc << '\n';
        options["help"] = true;
        return options;
    }

    if (vm.count("nettype"))
    {
        options["nettype"] 
            = static_cast<network_type>(vm["nettype"].as<size_t>());

        options["blockchain_path"] = xmreg::get_default_lmdb_folder(
                any_cast<network_type>(options["nettype"]));
    }

    if (vm.count("blockchain-path"))
    {
        options["blockchain_path"]
                = vm["blockchain-path"].as<string>();
    }

    if (vm.count("da-path"))
    {
        options["default_addresses_path"]
                = vm["da-path"].as<string>();
    }
    
    if (vm.count("config-path"))
    {
        options["config_path"]
                = vm["config-path"].as<string>();
    }
    
    options["port"] = vm["port"].as<size_t>();
    options["fiberpool_threads"] = vm["fiberthreads"].as<size_t>();
}
catch (po::error const& ex)
{
    cerr << ex.what() << '\n';
    options["error"] = true;
}

return options;
}

// based on https://stackoverflow.com/a/42274022
filesystem::path 
expand (filesystem::path in) 
{
    string s = in.string();
    if (s.size() < 1) 
        return in;

    if (s[0] != '~')
       return in; 

    const char * home = getenv("HOME");

    if (home == NULL) 
    {
      cerr << "error: HOME variable not set." 
           << endl;
      throw std::invalid_argument(
              "error: HOME environment variable not set.");
    }

    s = string(home) + s.substr(1, s.size () - 1);

    return filesystem::path (s);
}


}
