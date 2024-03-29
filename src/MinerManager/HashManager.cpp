// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

////////////////////////////////////
#include "MinerManager/HashManager.h"
////////////////////////////////////

#include <iostream>
#include <sstream>

#include "Utilities/ColouredMsg.h"

HashManager::HashManager(
    const std::shared_ptr<PoolCommunication> pool):
    m_pool(pool)
{
}

bool isHashValidForTarget(
    const std::vector<uint8_t> &hash,
    const uint64_t target)
{
    return *reinterpret_cast<const uint64_t *>(hash.data() + 24) < target;
}

void HashManager::submitHash(
    const std::vector<uint8_t> &hash,
    const std::string jobID,
    const uint32_t nonce,
    const uint64_t target)
{
    if (m_totalHashes == 0)
    {
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    m_totalHashes++;

    if (isHashValidForTarget(hash, target))
    {
        m_submittedHashes++;
        m_pool->submitShare(hash, jobID, nonce);
    }
}

void HashManager::shareAccepted()
{
    m_acceptedHashes++;

    m_pool->printPool();

    std::cout << SuccessMsg("Share accepted by pool!");

    std::stringstream stream;

    stream << " [" << m_acceptedHashes << " / " << m_submittedHashes << "]" << std::endl;

    std::cout << InformationMsg(stream.str());
}

void HashManager::printStats()
{
    const auto elapsedTime = std::chrono::high_resolution_clock::now() - m_startTime;

    /* Calculating in milliseconds for more accuracy */
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

    m_pool->printPool();

    std::cout << WhiteMsg("Hashrate: ");

    if (milliseconds != 0 && m_totalHashes != 0)
    {
        const double hashratePerSecond = (1000 * static_cast<double>(m_totalHashes) / milliseconds);
        std::cout << WhiteMsg(hashratePerSecond) << WhiteMsg(" H/s") << std::endl;
    }
    else
    {
        std::cout << WhiteMsg("N/A") << std::endl;
    }

    double submitPercentage = 0;

    if (m_acceptedHashes != 0 && m_submittedHashes != 0)
    {
        submitPercentage = 100 * (static_cast<double>(m_acceptedHashes) / m_submittedHashes);
    }

    m_pool->printPool();

    std::cout << WhiteMsg("Accepted shares percentage: ")
              << std::fixed << std::setprecision(2)
              << WhiteMsg(submitPercentage) << WhiteMsg("%") << std::endl;
}
