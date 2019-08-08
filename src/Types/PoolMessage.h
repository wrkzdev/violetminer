// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>

struct Job
{
    /* The mining job to work on */
    std::string blob;

    /* Identifier for this job */
    std::string jobID;

    /* The difficulty above which to submit shares */
    uint64_t shareDifficulty;

    /* The height of the block we are attempting to form */
    uint64_t height;

    /* The block major version. Duh. */
    uint8_t blockMajorVersion;

    /* Yeah, you can figure this one out champ. */
    uint8_t blockMinorVersion;

    uint8_t rootMajorVersion;

    uint8_t rootMinorVersion;
};

struct PoolMessage
{
    /* The same id we sent in the original message */
    std::string ID;

    /* The version of json_rpc the server is using */
    std::string jsonRpc;

    /* Potential error from the operation */
    std::optional<std::string> error;
};

struct LoginMessage : PoolMessage
{
    /* The ID to use to authenticate us as, well, us */
    std::string loginID;

    /* Whether the operation succeeded */
    std::string status;

    Job job;
};

inline void from_json(const nlohmann::json &j, Job &job)
{
    job.blob = j.at("blob").get<std::string>();
    job.jobID = j.at("job_id").get<std::string>();

    const std::string target = j.at("target").get<std::string>();

    job.shareDifficulty = std::stoull(target, nullptr, 16);

    job.height = j.at("height").get<uint64_t>();

    job.blockMajorVersion = j.at("blockMajorVersion").get<uint8_t>();
    job.blockMinorVersion = j.at("blockMinorVersion").get<uint8_t>();
    job.rootMajorVersion = j.at("rootMajorVersion").get<uint8_t>();
    job.rootMinorVersion = j.at("rootMinorVersion").get<uint8_t>();
}

inline void from_json(const nlohmann::json &j, PoolMessage &p)
{
    p.ID = j.at("id").get<std::string>();
    p.jsonRpc = j.at("jsonrpc").get<std::string>();

    const auto error = j.at("error");

    if (!error.is_null())
    {
        p.error = error.get<std::string>();
    }
}

inline void from_json(const nlohmann::json &j, LoginMessage &l)
{
    from_json(j, static_cast<PoolMessage &>(l));

    const auto result = j.at("result");

    l.loginID = result.at("id").get<std::string>();
    l.status = result.at("status").get<std::string>();
    l.job = result.at("job").get<Job>();
}
