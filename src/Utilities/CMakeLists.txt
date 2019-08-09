# Add the files we want to link against
set(utilities_source_files
    Console.cpp
    Input.cpp
    String.cpp
)

# Add the library to be linked against, with the previously specified source files
add_library(Utilities ${utilities_source_files})
