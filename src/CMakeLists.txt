# Add the ArgonVariants directory to add that library
add_subdirectory(ArgonVariants)

# Add the Argon2 directory to add that library
add_subdirectory(Argon2)

# Add the Miner directory to add the actual executable
add_subdirectory(Miner)

# Add the MinerManager directory to add that library
add_subdirectory(MinerManager)

# Add the PoolCommunication directory to add that library
add_subdirectory(PoolCommunication)

# Add the Types directory to add that library
add_subdirectory(Types)

# Add the Utilities directory to add that library
add_subdirectory(Utilities)

# Add the Argon2 directory so ArgonVariants can include it. Probably a better way to do this.
target_include_directories(ArgonVariants PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Argon2/src")

# Add the MinerManager directory as an include directory, so we can #include "MinerManager/MinerManager.h"
target_include_directories(MinerManager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the PoolCommunication directory as an include directory, so we can #include "PoolCommunication/PoolCommunication.h"
target_include_directories(PoolCommunication PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(Types PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the PoolCommunication directory as an include directory, so we can #include "Utilities/ColouredMsg.h"
target_include_directories(Utilities PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
