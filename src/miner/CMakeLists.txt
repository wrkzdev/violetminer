# Add an executable called test with main.cpp as the entrypoint
add_executable(violetminer main.cpp)

# Link violetminer to the Argon2 library
target_link_libraries(violetminer Argon2 Blake2)
