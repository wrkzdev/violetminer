// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>

#include "Types/IHashingAlgorithm.h"

struct Pool
{
    /* Host of the pool */
    std::string host;

    /* Port of the pool */
    uint16_t port;

    /* Username to login with */
    std::string username;

    /* Optional password to login with */
    std::string password;

    /* Optional rig identifier */
    std::string rigID;

    /* The mining algorithm to use with this pool */
    std::string algorithm;

    /* The string we use to authenticate us once we have logged in */
    std::string loginID;

    /* Gets an instance of the mining algorithm used for this pool */
    std::function<std::shared_ptr<IHashingAlgorithm>(void)> algorithmGenerator;
};
