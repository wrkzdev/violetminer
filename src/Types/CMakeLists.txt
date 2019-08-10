# Add the files we want to link against
set(types_source_files
    PoolMessage.cpp
)

# Add the library to be linked against, with the previously specified source files
add_library(Types ${types_source_files})
