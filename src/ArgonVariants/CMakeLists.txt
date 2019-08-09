# Add the files we want to link against
set(argon_variants_source_files
    Argon2Hash.cpp
)

# Add the library to be linked against, with the previously specified source files
add_library(ArgonVariants ${argon_variants_source_files})
