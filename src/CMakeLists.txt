# Don't build the Argon2 test executable
set(BUILD_TESTS OFF)

# Add the Argon2 directory to add that library
add_subdirectory(Argon2)

# Add the miner directory to add the actual executable
add_subdirectory(miner)

# Add the Argon2 directory as an include directory, so we can #include <Argon2/Argon2.h>
target_include_directories(Argon2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
