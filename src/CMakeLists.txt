# Add the Argon2 directory to add that library
add_subdirectory(Argon2)

# Add the Miner directory to add the actual executable
add_subdirectory(Miner)

# Add the PoolCommunication directory to add that library
add_subdirectory(PoolCommunication)

# Add the Utilities directory to add that library
add_subdirectory(Utilities)

# Add the PoolCommunication directory as an include directory, so we can #include "PoolCommunication/PoolCommunication.h"
target_include_directories(PoolCommunication PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the PoolCommunication directory as an include directory, so we can #include "Utilities/ColouredMsg.h"
target_include_directories(Utilities PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
