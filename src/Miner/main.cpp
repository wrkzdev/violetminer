// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#include <iostream>

#include "Config/Constants.h"
#include "MinerManager/MinerManager.h"
#include "PoolCommunication/PoolCommunication.h"
#include "Types/Pool.h"
#include "Utilities/ColouredMsg.h"
#include "ArgonVariants/Variants.h"
#include "Miner/GetConfig.h"

std::vector<Pool> getDevPools()
{
    std::vector<Pool> pools;

    Pool pool1;
    pool1.host = "127.0.0.1";
    pool1.port = 5555;
    pool1.username = "TRTLv2Fyavy8CXG8BPEbNeCHFZ1fuDCYCZ3vW5H5LXN4K2M2MHUpTENip9bbavpHvvPwb4NDkBWrNgURAd5DB38FHXWZyoBh4wW";
    pool1.algorithm = "chukwa";
    pool1.algorithmGenerator = ArgonVariant::Algorithms[pool1.algorithm];

    pools.push_back(pool1);

    return pools;
}

int main(int argc, char **argv)
{
    /* Get the pools, algorithm, etc from the user in some way */
    MinerConfig config = getMinerConfig(argc, argv);

    const auto userPoolManager = std::make_shared<PoolCommunication>(config.pools);

    /* Get the dev pools */
    std::vector<Pool> devPools = getDevPools();

    const auto devPoolManager = std::make_shared<PoolCommunication>(devPools);

    /* Setup a manager for the user pools and the dev pools */
    MinerManager userMinerManager(userPoolManager, config.threadCount);
    MinerManager devMinerManager(devPoolManager, config.threadCount);

    /* A cycle lasts 100 minutes */
    const auto cycleLength = std::chrono::minutes(100);

    /* We mine for the dev for DEV_FEE_PERCENT off the 100 minutes */
    const auto devMiningTime = std::chrono::seconds(static_cast<uint8_t>(60 * Constants::DEV_FEE_PERCENT));

    /* We mine for the user for the rest of the time */
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

            /* Mine for devMiningTime seconds */
            std::this_thread::sleep_for(devMiningTime);

            /* Stop mining for the dev. */
            devMinerManager.stop();

            std::cout << InformationMsg("=== Regular mining resumed. Thank you for supporting violetminer! ===") << std::endl;
        }
    }
}
