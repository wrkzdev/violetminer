// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "ArgonVariants/Argon2Hash.h"
#include "Types/IHashingAlgorithm.h"

namespace ArgonVariant
{
    inline std::function<std::shared_ptr<IHashingAlgorithm>(void)> chukwa = [](){
        return std::make_shared<Argon2Hash>(
            512,
            3,
            1,
            16,
            Constants::ARGON2ID
        );
    };

    inline std::function<std::shared_ptr<IHashingAlgorithm>(void)> chukwa_wrkz = [](){
        return std::make_shared<Argon2Hash>(
            256,
            4,
            1,
            16,
            Constants::ARGON2ID
        );
    };

    inline std::unordered_map<std::string, std::function<std::shared_ptr<IHashingAlgorithm>(void)>> Algorithms
    {
        { "chukwa", chukwa },
        { "turtlecoin", chukwa },
        { "chukwa_wrkz", chukwa_wrkz },
        { "wrkzcoin", chukwa_wrkz },
    };
}
