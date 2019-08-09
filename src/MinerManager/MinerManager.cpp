// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

//////////////////////////////////////
#include "MinerManager/MinerManager.h"
//////////////////////////////////////

MinerManager::MinerManager(
    const std::shared_ptr<PoolCommunication> pool,
    const HashingAlgorithm hashingAlgorithm,
    const uint32_t threadCount):
    m_pool(pool),
    m_hashingAlgorithm(hashingAlgorithm),
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
}
