include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Argon2/src")

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

# Add the current directory as an include directory for ArgonVariants
target_include_directories(ArgonVariants PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the current directory as an include directory for MinerManager
target_include_directories(MinerManager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the current directory as an include directory for PoolCommunication
target_include_directories(PoolCommunication PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the current directory as an include directory for Types
target_include_directories(Types PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Add the current directory as an include directory for Utilities
target_include_directories(Utilities PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
