// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

//////////////////////////////
#include "Types/PoolMessage.h"
//////////////////////////////

std::variant<
    JobMessage,
    ErrorMessage,
    LoginMessage,
    ShareAcceptedMessage
> parsePoolMessage(const std::string &message)
{
    try
    {
        const JobMessage result = nlohmann::json::parse(message);
        return result;
    }
    catch (const std::exception &)
    {
    }

    try
    {
        const ShareAcceptedMessage result = nlohmann::json::parse(message);
        return result;
    }
    catch (const std::exception &)
    {
    }

    try
    {
    }
    catch (const std::exception &)
    {
        const LoginMessage result = nlohmann::json::parse(message);
        return result;
    }

    try
    {
        const ErrorMessage result = nlohmann::json::parse(message);
        return result;
    }
    catch (const std::exception &)
    {
    }

    throw std::invalid_argument("Failed to parse pool message: " + message);
}
