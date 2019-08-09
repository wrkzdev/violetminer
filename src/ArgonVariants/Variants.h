// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <functional>
#include <memory>

#include "ArgonVariants/Argon2Hash.h"
#include "Types/IHashingAlgorithm.h"

namespace ArgonVariant
{
    std::function<std::shared_ptr<IHashingAlgorithm>(void)> chukwa = [](){
        return std::make_shared<Argon2Hash>(
            512,
            3,
            1,
            16,
            Constants::ARGON2ID
        );
    };
}
