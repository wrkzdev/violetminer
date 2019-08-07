# Add the files we want to link against
set(pool_communication_source_files
    PoolCommunication.cpp
)

# Add the library to be linked against, with the previously specified source files
add_library(PoolCommunication ${pool_communication_source_files})
