# Add the files we want to link against
set(miner_manager_source_files
    HashManager.cpp
    MinerManager.cpp
)

# Add the library to be linked against, with the previously specified source files
add_library(MinerManager ${miner_manager_source_files})
