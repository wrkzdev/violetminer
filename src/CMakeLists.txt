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

# Add the Utilities directory to add that library
add_subdirectory(Utilities)

# Add the Argon2 directory as an include directory, so we can #include <Argon2/Argon2.h>
target_include_directories(Argon2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the ArgonVariants directory as an include directory, so we can #include "ArgonVariants/Argon2Hash.h"
target_include_directories(ArgonVariants PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the MinerManager directory as an include directory, so we can #include "MinerManager/MinerManager.h"
target_include_directories(MinerManager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the PoolCommunication directory as an include directory, so we can #include "PoolCommunication/PoolCommunication.h"
target_include_directories(PoolCommunication PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the PoolCommunication directory as an include directory, so we can #include "Utilities/ColouredMsg.h"
target_include_directories(Utilities PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()

# Link Argon2 include dirs to ArgonVariants
target_link_libraries(ArgonVariants Argon2)
