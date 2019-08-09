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

                        if (message.error)
                        {
                            loginFailed(pool, i, false, message.error->errorMessage);
                            continue;
                        }

                        std::cout << InformationMsg(formatPool(pool)) << SuccessMsg("Logged in.") << std::endl;

                        m_currentPool = pool;
                        m_preferredPool = i == 1;

                        return;

                    }
                    catch (const std::exception &e)
                    {
                        loginFailed(pool, i, false, "Failed to parse message from pool (" + std::string(e.what()) + ") (" + *res + ")");
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
