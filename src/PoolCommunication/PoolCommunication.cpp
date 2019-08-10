// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

////////////////////////////////////////////////
#include "PoolCommunication/PoolCommunication.h"
////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <thread>

#include "Config/Constants.h"
#include "ExternalLibs/json.hpp"
#include "SocketWrapper/SocketWrapper.h"
#include "Types/PoolMessage.h"
#include "Utilities/ColouredMsg.h"

PoolCommunication::PoolCommunication(
    const std::vector<Pool> allPools):
    m_allPools(allPools)
{
}

std::string formatPool(const Pool pool)
{
    return "[" + pool.host + ":" + std::to_string(pool.port) + "] ";
}

void loginFailed(
    const Pool pool,
    const int loginAttempt,
    const bool connectFail,
    const std::string customMessage = "")
{
    std::stringstream stream;

    stream << "Failed to " << (connectFail ? "connect" : "login")
           << " to pool, attempt ";

    std::cout << InformationMsg(formatPool(pool)) << WarningMsg(stream.str())
              << InformationMsg(loginAttempt)
              << InformationMsg("/")
              << InformationMsg(Constants::MAX_LOGIN_ATTEMPTS) << std::endl;

    if (customMessage != "")
    {
        std::cout << InformationMsg(formatPool(pool))
                  << WarningMsg("Error: " + customMessage) << std::endl;
    }

    if (loginAttempt != Constants::MAX_LOGIN_ATTEMPTS)
    {
        std::cout << InformationMsg(formatPool(pool)) << "Will try again in "
                  << InformationMsg(Constants::POOL_LOGIN_RETRY_INTERVAL / 1000)
                  << " seconds." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(Constants::POOL_LOGIN_RETRY_INTERVAL));
}

void PoolCommunication::printPool()
{
    std::cout << InformationMsg(formatPool(m_currentPool));
}

void PoolCommunication::login()
{
    while (true)
    {
        for (const auto &pool : m_allPools)
        {
            m_socket = std::make_shared<sockwrapper::SocketWrapper>(
                pool.host.c_str(), pool.port, '\n', Constants::POOL_LOGIN_RETRY_INTERVAL / 1000
            );

            std::stringstream stream;

            std::cout << InformationMsg(formatPool(pool)) << "Attempting to connect to pool..." << std::endl;

            for (int i = 1; i <= Constants::MAX_LOGIN_ATTEMPTS; i++)
            {
                const bool success = m_socket->start();

                if (!success)
                {
                    loginFailed(pool, i, true);
                    continue;
                }

                const nlohmann::json loginMsg = {
                    {"method", "login"},
                    {"params", {
                        {"login", pool.username},
                        {"pass", pool.password},
                        {"rigid", pool.rigID},
                    }},
                    {"id", "1"}
                };

                const auto res = m_socket->sendMessageAndGetResponse(loginMsg.dump() + "\n");

                if (res)
                {
                    try
                    {
                        const LoginMessage message = nlohmann::json::parse(*res);

                        std::cout << InformationMsg(formatPool(pool)) << SuccessMsg("Logged in.") << std::endl;

                        m_currentPool = pool;
                        m_currentPool.loginID = message.loginID;
                        m_preferredPool = i == 1;
                        m_currentJob = message.job;

                        return;

                    }
                    catch (const std::exception &e)
                    {
                        try
                        {
                            /* Failed to parse as LoginMessage. Maybe it's an error message? */
                            const ErrorMessage message = nlohmann::json::parse(*res);
                            loginFailed(pool, i, false, message.error.errorMessage);
                        }
                        catch (const std::exception &e)
                        {
                            loginFailed(pool, i, false, "Failed to parse message from pool (" + std::string(e.what()) + ") (" + *res + ")");
                        }

                        continue;
                    }
                }
                else
                {
                    loginFailed(pool, i, false);
                    continue;
                }
            }

            std::cout << InformationMsg(formatPool(pool)) << WarningMsg("All login/connect attempts failed. Trying next pool.") << std::endl;
        }

        std::cout << InformationMsg(formatPool(m_allPools[0])) << WarningMsg("Failed to login/connect to all specified pools. Retrying first pool.") << std::endl;
    }
}

Job PoolCommunication::getJob()
{
    return m_currentJob;
}

void PoolCommunication::submitShare(
    const std::vector<uint8_t> &hash,
    const std::string jobID,
    const uint32_t nonce)
{
    const nlohmann::json submitMsg = {
        {"method", "submit"},
        {"params", {
            {"id", m_currentPool.loginID},
            {"job_id", jobID},
            {"nonce", Utilities::toHex(nonce)},
            {"result", Utilities::toHex(hash)},
            {"rig_id", m_currentPool.rigID},
        }},
        {"id", "1"}
    };

    m_socket->sendMessage(submitMsg.dump() + "\n");
}

void PoolCommunication::onNewJob(std::function<void(const Job &job)> callback)
{
    m_onNewJob = callback;
}

void PoolCommunication::onHashAccepted(std::function<void(const std::string &shareID)> callback)
{
    m_onHashAccepted = callback;
}

void PoolCommunication::handleMessages()
{
    m_socket->onMessage([this](const std::string &message) {
        //std::cout << "Received message: " << message << std::endl;

        try
        {
            const auto poolMessage = parsePoolMessage(message);

            if (std::holds_alternative<JobMessage>(poolMessage))
            {
                const auto job = std::get<JobMessage>(poolMessage).job;
                m_currentJob = job;

                if (m_onNewJob)
                {
                    m_onNewJob(job);
                }
            }
            else if (std::holds_alternative<ShareAcceptedMessage>(poolMessage))
            {
                const auto shareAccepted = std::get<ShareAcceptedMessage>(poolMessage);

                if (shareAccepted.status == "OK" && m_onHashAccepted)
                {
                    m_onHashAccepted(shareAccepted.ID);
                }
            }
            else if (std::holds_alternative<ErrorMessage>(poolMessage))
            {
                const auto error = std::get<ErrorMessage>(poolMessage).error.errorMessage;

                std::cout << InformationMsg("Error message received from pool: ") << WarningMsg(error) << std::endl;

                if (error == "Low difficulty share")
                {
                    std::cout << WarningMsg("Please ensure you are using the correct mining algorithm for this pool.") << std::endl;
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cout << WarningMsg(e.what()) << std::endl;
        }
    });
}
