//
// Created by mwo on 10/07/18.
//

#include "BlockchainSetup.h"

#include "src/tools.h"

namespace xmreg
{

BlockchainSetup::BlockchainSetup(
        network_type _net_type,
        string _config_path)
        : net_type {_net_type},
          config_path {_config_path}
{
    config_json = read_config(config_path);

    _init();
}


BlockchainSetup::BlockchainSetup(
        network_type _net_type,
        nlohmann::json _config)
    : net_type {_net_type},
      config_json (_config)
{
    _init();
}


void
BlockchainSetup::get_blockchain_path()
{
    switch (net_type)
    {
        case network_type::MAINNET:
            blockchain_path = config_json["blockchain-path"]["mainnet"]
                    .get<string>();
            deamon_url = config_json["daemon-url"]["mainnet"];
            break;
        case network_type::TESTNET:
            blockchain_path = config_json["blockchain-path"]["testnet"]
                    .get<string>();
            deamon_url = config_json["daemon-url"]["testnet"];
            break;
        case network_type::STAGENET:
            blockchain_path = config_json["blockchain-path"]["stagenet"]
                    .get<string>();
            deamon_url = config_json["daemon-url"]["stagenet"];
            break;
        default:
            throw std::runtime_error("Invalid netowork type provided "
                               + std::to_string(static_cast<int>(net_type)));
    }

    if (!xmreg::get_blockchain_path(blockchain_path, net_type))
        throw std::runtime_error("Error getting blockchain path.");
}



string
BlockchainSetup::get_network_name(network_type n_type)
{
    switch (n_type)
    {
        case network_type::MAINNET:
            return "mainnet";
        case network_type::TESTNET:
            return "testnet";
        case network_type::STAGENET:
            return "stagenet";
        default:
            throw std::runtime_error("wrong network");
    }
}


nlohmann::json
BlockchainSetup::read_config(string config_json_path)
{
    // check if config-file provided exist
    if (!boost::filesystem::exists(config_json_path))
        throw std::runtime_error("Config file " + config_json_path
                                 + " does not exist");

    nlohmann::json _config;

    try
    {
        // try reading and parsing json config file provided
        std::ifstream i(config_json_path);
        i >> _config;
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(string{"Cant parse json string as json: "}
                                 + e.what());
    }

    return _config;
}



    void
BlockchainSetup::_init()
{
    refresh_block_status_every
            = seconds {config_json["refresh_block_status_every_seconds"]};
    blocks_search_lookahead
            = config_json["blocks_search_lookahead"];
    max_number_of_blocks_to_import
            = config_json["max_number_of_blocks_to_import"];
    fiberpool_threads_count 
            = config_json["fiberpool_threads_count"];
    websockets_threads_count 
            = config_json["websockets_threads_count"];

    get_blockchain_path();
}

}
