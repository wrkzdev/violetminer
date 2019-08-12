// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

////////////////////////////
#include "Miner/GetConfig.h"
////////////////////////////

#include <fstream>

#include "Config/Constants.h"
#include "ExternalLibs/cxxopts.hpp"
#include "ExternalLibs/json.hpp"
#include "Utilities/ColouredMsg.h"
#include "Utilities/Console.h"
#include "Utilities/Input.h"
#include "Utilities/String.h"

void to_json(nlohmann::json &j, const MinerConfig &config)
{
    j = {
        {"threadCount", config.threadCount},
        {"pools", config.pools}
    };
}

void from_json(const nlohmann::json &j, MinerConfig &config)
{
    config.pools = j.at("pools").get<std::vector<Pool>>();

    if (j.find("threadCount") != j.end())
    {
        config.threadCount = j.at("threadCount").get<uint32_t>();
    }
}

Pool getPool()
{
    Pool pool;

    while (true)
    {
        std::cout << InformationMsg("Enter the pool address to mine to.") << std::endl
                  << InformationMsg("This should look something like trtl.pool.mine2gether.com:3335: ");

        std::string address;
        std::string host;
        uint16_t port;

        std::getline(std::cin, address);

        Utilities::trim(address);

        if (address == "")
        {
            continue;
        }

        if (!Utilities::parseAddressFromString(host, port, address))
        {
            std::cout << WarningMsg("Invalid pool address! Should be in the form host:port, for example, trtl.pool.mine2gether.com:3335!")
                      << std::endl;

            continue;
        }

        pool.host = host;
        pool.port = port;

        break;
    }

    while (true)
    {
        std::cout << InformationMsg("\nEnter your pool login. This is usually your wallet address: ");

        std::string login;

        std::getline(std::cin, login);

        Utilities::trim(login);

        if (login == "")
        {
            std::cout << WarningMsg("Login cannot be empty! Try again.") << std::endl;
            continue;
        }

        pool.username = login;

        break;
    }

    std::cout << InformationMsg("\nEnter the pool password. You can usually leave this blank, or use 'x': ");

    std::string password;

    std::getline(std::cin, password);

    pool.password = password;

    while (true)
    {
        std::cout << InformationMsg("Available mining algorithms:") << std::endl;

        for (const auto [algorithm, hashingFunc] : ArgonVariant::Algorithms)
        {
            std::cout << SuccessMsg("* ") << SuccessMsg(algorithm) << std::endl;
        }

        std::cout << InformationMsg("\nEnter the algorithm you wish to mine with on this pool: ");

        std::string algorithm;

        std::getline(std::cin, algorithm);

        Utilities::trim(algorithm);

        std::transform(algorithm.begin(), algorithm.end(), algorithm.begin(), ::tolower);

        if (algorithm == "")
        {
            continue;
        }

        const auto it = std::find_if(
            ArgonVariant::Algorithms.begin(), 
            ArgonVariant::Algorithms.end(),
            [&algorithm](const auto algo)
        {
            std::string theirAlgo = algo.first;
            std::transform(theirAlgo.begin(), theirAlgo.end(), theirAlgo.begin(), ::tolower);
            return theirAlgo == algorithm;
        });

        if (it != ArgonVariant::Algorithms.end())
        {
            pool.algorithm = it->first;
            pool.algorithmGenerator = it->second;
            break;
        }
        else
        {
            std::cout << WarningMsg("Unknown algorithm \"" + algorithm + "\". Try again.") << std::endl;
        }
    }

    std::cout << InformationMsg("\nEnter the rig ID to use with this pool. This can identify your different computers to the pool.") << std::endl
              << InformationMsg("You can leave this blank if desired: ");

    std::string rigID;

    std::getline(std::cin, rigID);

    pool.rigID = rigID;

    return pool;
}

std::vector<Pool> getPools()
{
    std::vector<Pool> pools;

    while (true)
    {
        const Pool pool = getPool();

        pools.push_back(pool);

        if (!Utilities::confirm("\nWould you like to add another pool?", false))
        {
            break;
        }

        std::cout << std::endl;
    }

    return pools;
}

MinerConfig getConfigInteractively()
{
    MinerConfig config;

    config.pools = getPools();

    std::ofstream configFile(Constants::CONFIG_FILE_NAME);

    if (configFile)
    {
        nlohmann::json j = config;
        configFile << j.dump(4) << std::endl;
        std::cout << SuccessMsg("Wrote config file to " + Constants::CONFIG_FILE_NAME) << std::endl;
    }

    return config;
}

MinerConfig getConfigFromJSON(const std::string &configLocation)
{
    std::ifstream configFile(configLocation);

    if (!configFile)
    {
        std::stringstream stream;

        stream << "Failed to open config file \"" << configLocation
               << "\"." << std::endl << "Does the file exist?" << std::endl;

        std::cout << WarningMsg(stream.str());

        Console::exitOrWaitForInput(1);
    }

    try
    {
        std::string fileContents((std::istreambuf_iterator<char>(configFile)),
                                 (std::istreambuf_iterator<char>()));

        const MinerConfig jsonConfig = nlohmann::json::parse(fileContents);

        return jsonConfig;
    }
    catch (const nlohmann::json::exception &e)
    {
        std::cout << WarningMsg("Failed to parse config file: ")
                  << WarningMsg(e.what()) << std::endl
                  << "Try pasting your config file (" << configLocation << ") into "
                  << InformationMsg("https://jsonformatter.curiousconcept.com/")
                  << " to figure out which line is invalid." << std::endl;

        Console::exitOrWaitForInput(1);
    }
    catch (const std::invalid_argument &e)
    {
        std::cout << WarningMsg("Config file is invalid: ")
                  << WarningMsg(e.what()) << std::endl;

        Console::exitOrWaitForInput(1);
    }
    catch (const std::exception &e)
    {
        std::cout << WarningMsg("Failed to read from config file: ")
                  << WarningMsg(e.what()) << std::endl;

        Console::exitOrWaitForInput(1);
    }

    /* Compiler can't figure out that Console::exitOrWaitForInput() exits the
       program */
    throw std::runtime_error("Programmer error");
}

MinerConfig getMinerConfig(int argc, char **argv)
{
    MinerConfig config;
    Pool poolConfig;

    std::string poolAddress;

    bool help;
    bool version;

    cxxopts::Options options(argv[0], "");

    options.add_options("Core")
        ("h,help", "Display this help message",
         cxxopts::value<bool>(help)->implicit_value("true"))

        ("v,version", "Display the miner version",
         cxxopts::value<bool>(version)->implicit_value("true"))

        ("config", "The location of the config file to use",
         cxxopts::value<std::string>(config.configLocation), "<file>");

    options.add_options("Pool")
        ("pool", "The pool <host:port> combination to mine to",
         cxxopts::value<std::string>(poolAddress), "<host:port>")

        ("username", "The username to use with the pool, normally your wallet address",
         cxxopts::value<std::string>(poolConfig.username), "<username>")

        ("password", "The password to use with the pool, can be omitted",
         cxxopts::value<std::string>(poolConfig.password), "<password>")

        ("rigid", "The rig ID to use with the pool, can be omitted",
         cxxopts::value<std::string>(poolConfig.rigID), "<rig ID>");

    options.add_options("Miner")
        ("algorithm", "The mining algorithm to use",
         cxxopts::value<std::string>(poolConfig.algorithm), "<algorithm>")

        ("threads", "The number of mining threads to use",
         cxxopts::value<uint32_t>(config.threadCount)->default_value(std::to_string(config.threadCount)), "<threads>");

    try
    {
        const auto result = options.parse(argc, argv);

        if (help)
        {
            std::cout << options.help({}) << std::endl;
            exit(0);
        }

        if (version)
        {
            std::cout << "violetminer " << Constants::VERSION << std::endl;
            exit(0);
        }

        const bool configFileExists = static_cast<bool>(std::ifstream(Constants::CONFIG_FILE_NAME));

        /* Use config file if no args given and config file exists on disk */
        if (configFileExists && result.arguments().size() == 0)
        {
            config.configLocation = Constants::CONFIG_FILE_NAME;
        }

        /* If we have been given a config file, or no args are given and config
           file exists on disk */
        if (config.configLocation != "")
        {
            return getConfigFromJSON(config.configLocation);
        }
        /* No command line args given, and no config on disk, create config from
           user input */
        else if (result.arguments().size() == 0)
        {
            return getConfigInteractively();
        }
        else
        {
            const std::vector<std::string> requiredArgs { "pool", "username", "algorithm" };

            for (const auto &arg : requiredArgs)
            {
                if (result.count(arg) == 0)
                {
                    std::cout << WarningMsg("Required argument --" + arg + " has not been supplied!") << std::endl;
                    Console::exitOrWaitForInput(1);
                }
            }

            if (!Utilities::parseAddressFromString(poolConfig.host, poolConfig.port, poolAddress))
            {
                std::cout << WarningMsg("Failed to parse pool address!") << std::endl;
                Console::exitOrWaitForInput(1);
            }

            if (poolConfig.username == "")
            {
                std::cout << WarningMsg("Username cannot be empty!") << std::endl;
                Console::exitOrWaitForInput(1);
            }

            const auto it = ArgonVariant::Algorithms.find(poolConfig.algorithm);

            if (it == ArgonVariant::Algorithms.end())
            {
                std::cout << WarningMsg("Algorithm \"" + poolConfig.algorithm + "\" is not a known algorithm!") << std::endl;

                std::cout << InformationMsg("Available mining algorithms:") << std::endl;

                for (const auto [algorithm, hashingFunc] : ArgonVariant::Algorithms)
                {
                    std::cout << SuccessMsg("* ") << SuccessMsg(algorithm) << std::endl;
                }

                Console::exitOrWaitForInput(1);
            }

            poolConfig.algorithmGenerator = it->second;

            config.pools.push_back(poolConfig);

            return config;
        }
    }
    catch (const cxxopts::OptionException &e)
    {
        std::cout << WarningMsg("Error: Unable to parse command line options: ") << WarningMsg(e.what())
                  << std::endl << std::endl
                  << options.help({}) << std::endl;

        Console::exitOrWaitForInput(1);
    }

    /* Compiler can't figure out that Console::exitOrWaitForInput() exits the
       program */
    throw std::runtime_error("Programmer error");
}
