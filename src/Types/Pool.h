// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <functional>
#include <string>

#include "ArgonVariants/Variants.h"
#include "Config/Constants.h"
#include "ExternalLibs/json.hpp"
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

    /* Custom user agent */
    std::string agent = "violetminer-" + Constants::VERSION;

    /* The string we use to authenticate us once we have logged in */
    std::string loginID;

    /* Whether to use nicehash style nonces */
    bool niceHash = false;

    /* Gets an instance of the mining algorithm used for this pool */
    std::function<std::shared_ptr<IHashingAlgorithm>(void)> algorithmGenerator;
};

inline void to_json(nlohmann::json &j, const Pool &pool)
{
    j = {
        {"host", pool.host},
        {"port", pool.port},
        {"username", pool.username},
        {"password", pool.password},
        {"rigID", pool.rigID},
        {"algorithm", pool.algorithm},
        {"agent", pool.agent},
        {"niceHash", pool.niceHash}
    };
}

inline void from_json(const nlohmann::json &j, Pool &pool)
{
    pool.host = j.at("host").get<std::string>();
    pool.port = j.at("port").get<uint16_t>();
    pool.username = j.at("username").get<std::string>();

    if (j.find("password") != j.end())
    {
        pool.password = j.at("password").get<std::string>();
    }

    if (j.find("rigID") != j.end())
    {
        pool.rigID = j.at("rigID").get<std::string>();
    }

    pool.algorithm = j.at("algorithm").get<std::string>();

    const auto it = ArgonVariant::Algorithms.find(pool.algorithm);

    if (it == ArgonVariant::Algorithms.end())
    {
        throw std::invalid_argument("Algorithm \"" + pool.algorithm + "\" is not a known algorithm!");
    }

    pool.algorithmGenerator = it->second;

    if (j.find("agent") != j.end())
    {
        pool.agent = j.at("agent").get<std::string>();
    }

    if (j.find("niceHash") != j.end())
    {
        pool.niceHash = j.at("niceHash").get<bool>();
    }
}
