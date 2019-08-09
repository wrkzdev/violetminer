// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

//////////////////////////////////////
#include "MinerManager/MinerManager.h"
//////////////////////////////////////

#include <iostream>

MinerManager::MinerManager(
    const std::shared_ptr<PoolCommunication> pool,
    const std::function<std::shared_ptr<IHashingAlgorithm>(void)> algorithmGenerator,
    const uint32_t threadCount):
    m_pool(pool),
    m_algorithmGenerator(algorithmGenerator),
    m_threadCount(threadCount)
{
}

MinerManager::~MinerManager()
{
    stop();
}

void MinerManager::start()
{
    m_shouldStop = false;

    m_startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < m_threadCount; i++)
    {
        m_threads.push_back(std::thread(&MinerManager::hash, this));
    }
}

void MinerManager::stop()
{
    m_shouldStop = true;

    for (auto &thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    m_threads.clear();
}

void MinerManager::hash()
{
    std::shared_ptr<IHashingAlgorithm> algorithm = m_algorithmGenerator();

    /* Let the algorithm perform any neccessary initialization */
    algorithm->init();

    const std::vector<uint8_t> chukwaInput = {
        1, 0, 251, 142, 138, 200, 5, 137, 147, 35, 55, 27, 183, 144, 219, 25,
        33, 138, 253, 141, 184, 227, 117, 93, 139, 144, 243, 155, 61, 85, 6,
        169, 171, 206, 79, 169, 18, 36, 69, 0, 0, 0, 0, 238, 129, 70, 212, 159,
        169, 62, 231, 36, 222, 181, 125, 18, 203, 198, 198, 243, 185, 36, 217,
        70, 18, 124, 122, 151, 65, 143, 147, 72, 130, 143, 15, 2
    };

    while (!m_shouldStop)
    {
        const auto hash = algorithm->hash(chukwaInput);
        m_hashes++;

        if (m_hashes % 5000 == 0)
        {
            std::cout << "Total hashes: " << m_hashes << ", hashrate: ";

            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - m_startTime
            ).count();

            if (seconds == 0)
            {
                seconds = 1;
            }

            std::cout << m_hashes / seconds << " H/s" << std::endl;
        }
    }
}
