// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

//////////////////////////////////////
#include "MinerManager/MinerManager.h"
//////////////////////////////////////

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Utilities/ColouredMsg.h"
#include "Utilities/Utilities.h"

MinerManager::MinerManager(
    const std::shared_ptr<PoolCommunication> pool,
    const std::function<std::shared_ptr<IHashingAlgorithm>(void)> algorithmGenerator,
    const uint32_t threadCount):
    m_pool(pool),
    m_algorithmGenerator(algorithmGenerator),
    m_threadCount(threadCount),
    m_hashManager(pool),
    m_gen(m_device())
{
}

MinerManager::~MinerManager()
{
    stop();
}

void MinerManager::start()
{
    m_shouldStop = false;
    m_currentJob = m_pool->getJob();
    m_nonce = m_distribution(m_gen);
    m_newJobAvailable = std::vector(m_threadCount, false);

    for (uint32_t i = 0; i < m_threadCount; i++)
    {
        m_threads.push_back(std::thread(&MinerManager::hash, this, i));
    }

    m_statsThread = std::thread(&MinerManager::printStats, this);

    m_pool->onNewJob([this](const Job &job){
        m_nonce = m_distribution(m_gen);
        m_currentJob = job;
        m_newJobAvailable = std::vector(m_threadCount, true);
        m_pool->printPool();
        std::cout << InformationMsg("New job, diff ") << SuccessMsg(job.shareDifficulty) << std::endl;
    });

    m_pool->onHashAccepted([this](const std::string &){
        m_hashManager.shareAccepted();
    });

    m_pool->handleMessages();
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

    if (m_statsThread.joinable())
    {
        m_statsThread.join();
    }
}

void MinerManager::hash(uint32_t threadNumber)
{
    std::shared_ptr<IHashingAlgorithm> algorithm = m_algorithmGenerator();

    /* Let the algorithm perform any necessary initialization */
    algorithm->init(m_currentJob.rawBlob);

    while (!m_shouldStop)
    {
        /* Offset the nonce by our thread number so each thread has an individual
           nonce */
        uint32_t localNonce = m_nonce + threadNumber;

        std::vector<uint8_t> job = m_currentJob.rawBlob;

        std::memcpy(job.data() + 39, &localNonce, sizeof(uint32_t));

        /* Allow the algorithm to reinitialize for the new round, as some algorithms
           can avoid reinitializing each round. For example, we can calculate
           the salt once per job, and cache it. */
        algorithm->reinit(job);

        while (!m_newJobAvailable[threadNumber])
        {
            const auto hash = algorithm->hash(job);

            m_hashManager.submitHash(hash, m_currentJob.jobID, localNonce, m_currentJob.target);

            localNonce += m_threadCount;

            std::memcpy(job.data() + 39, &localNonce, sizeof(uint32_t));
        }

        /* Switch to new job. */
        m_newJobAvailable[threadNumber] = false;
    }
}

void MinerManager::printStats()
{
    while (!m_shouldStop)
    {
        Utilities::sleepUnlessStopping(std::chrono::seconds(20), m_shouldStop);
        m_hashManager.printStats();
    }
}
