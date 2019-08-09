// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#include <iostream>

#include "Config/Constants.h"
#include "MinerManager/MinerManager.h"
#include "PoolCommunication/PoolCommunication.h"
#include "Types/Pool.h"
#include "Utilities/ColouredMsg.h"
#include "Utilities/Input.h"
#include "Utilities/String.h"

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

int main()
{
    std::vector<Pool> pools = getPools();

    const auto pool = std::make_shared<PoolCommunication>(pools);

    pool->login();

    MinerManager userMinerManager(pool, {}, std::thread::hardware_concurrency());
    MinerManager devMinerManager(pool, {}, std::thread::hardware_concurrency());

    const auto cycleLength = std::chrono::minutes(100);
    const auto devMiningTime = std::chrono::seconds(static_cast<uint8_t>(60 * Constants::DEV_FEE_PERCENT));
    const auto userMiningTime = cycleLength - devMiningTime;

    if (Constants::DEV_FEE_PERCENT == 0)
    {
        std::cout << WarningMsg("Dev fee disabled :( Consider making a one off donation to TRTLv1c5XpYGdwHbcU94foRojzLiz3pQ4AJN6swsy514QrydgvYPCKhDZPt61JG5eVGJKrHsXJHSUHDmAhZ134q8QRN2kJwUbtB") << std::endl;

        /* No dev fee, just start the users mining */
        userMinerManager.start();

        /* Wait forever */
        std::promise<void>().get_future().wait();
    }
    else
    {
        /* 100 minute rounds, alternating between users pool and devs pool */
        while (true)
        {
            /* Start mining for the user */
            userMinerManager.start();

            /* Mine for userMiningTime minutes */
            std::this_thread::sleep_for(userMiningTime);

            /* Stop mining for the user */
            userMinerManager.stop();

            std::cout << InformationMsg("=== Started mining to the development pool - Thank you for supporting violetminer! ===") << std::endl;
            std::cout << InformationMsg("=== This will last for " + std::to_string(devMiningTime.count()) + " seconds. (Every 100 minutes) ===") << std::endl;

            /* Start mining for the dev */
            devMinerManager.start();

            /* Mine for devMiningTime minutes */
            std::this_thread::sleep_for(devMiningTime);

            /* Stop mining for the dev. */
            devMinerManager.stop();

            std::cout << InformationMsg("=== Regular mining resumed. Thank you for supporting violetminer! ===") << std::endl;
        }
    }
}
