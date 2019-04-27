#include "utils.hpp"
#include "src/tools.h"

#include <boost/program_options.hpp>
#include "boost/algorithm/string.hpp"

#include <filesystem>

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
        cout << vm["nettype"].as<size_t>() << endl;
        
        options["nettype"] 
            = static_cast<network_type>(vm["nettype"].as<size_t>());

        options["blockchain_path"] = filesystem::path {
            xmreg::get_default_lmdb_folder(
                any_cast<network_type>(options["nettype"]))};
    }

    if (vm.count("blockchain-path"))
        options["blockchain_path"]
                = filesystem::path {vm["blockchain-path"].as<string>()};
    
    options["port"] = vm["port"].as<size_t>();
}
catch (po::error const& ex)
{
    cerr << ex.what() << '\n';
    options["error"] = true;
}

return options;
}


}
