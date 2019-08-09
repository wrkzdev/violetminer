set(OPENSSL_USE_STATIC_LIBS ON)

if (MSVC)
    set(OPENSSL_MSVC_STATIC_RT ON)
endif ()

# We have to look for Homebrew OpenSSL a bit differently
# Borrowed from https://github.com/tarantool/tarantool/commit/6eab201af1843f53a833c8928dc58fceffa08147
if (APPLE)
    find_program(HOMEBREW_EXECUTABLE brew)
    execute_process(COMMAND ${HOMEBREW_EXECUTABLE} --prefix openssl
            OUTPUT_VARIABLE HOMEBREW_OPENSSL
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (DEFINED HOMEBREW_OPENSSL)
        if (NOT DEFINED OPENSSL_ROOT_DIR)
            message(STATUS "Setting OpenSSL root to ${HOMEBREW_OPENSSL}")
            set(OPENSSL_ROOT_DIR "${HOMEBREW_OPENSSL}")
        endif ()
    endif ()
endif ()

# Try and find OpenSSL
find_package(OpenSSL)

# Didn't work. Lets try finding it with PkgConfig instead
if (NOT OPENSSL_FOUND)
    find_package(PkgConfig)

    if (PKG_CONFIG_FOUND)
        pkg_search_module(OPENSSL openssl)
    endif()
endif()

if (OPENSSL_FOUND)
    # On non MSVC build systems, we need to link ldl with the static OpenSSL library
    if (NOT MSVC)
        set(OPENSSL_LIBRARIES "${OPENSSL_LIBRARIES};dl")
    endif ()

    include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})

    # Enable SSL support with our SocketWrapper
    add_definitions(-DSOCKETWRAPPER_OPENSSL_SUPPORT)

    message(STATUS "OpenSSL Found: ${OPENSSL_INCLUDE_DIR}")
else ()
    message(STATUS "OpenSSL Found: No... Skipping...")
endif ()

# Add an executable called violetminer with main.cpp as the entrypoint
add_executable(violetminer main.cpp)

# Need to link against pthreads on non windows
if (NOT MSVC)
    target_link_libraries(violetminer pthread)
endif()

if (OPENSSL_FOUND)
    target_link_libraries(violetminer ${OPENSSL_LIBRARIES})
endif()

# Link violetminer to the libraries it uses
target_link_libraries(violetminer
    ArgonVariants
    Argon2
    Blake2
    MinerManager
    PoolCommunication
    Utilities)
