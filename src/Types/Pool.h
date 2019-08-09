// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>

struct Pool
{
    /* Host of the pool */
    std::string host;

    /* Port of the pool */
    uint16_t port;

    /* Username to login with */
    std::string username;

    /* Optional password to login with */
    std::string password = "x";

    /* Optional rig identifier */
    std::string rigID;
};
