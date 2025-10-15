cmake_minimum_required(VERSION 3.21)
project(ITKExplorer LANGUAGES C CXX)

# ----------------------------------------------------------------------------
# Global settings
# ----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Default to Release build if not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the build type: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# ----------------------------------------------------------------------------
# Global include paths
# ----------------------------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)

# ----------------------------------------------------------------------------
# Subdirectories (modular design)
# ----------------------------------------------------------------------------
# Each subdirectory corresponds to a logical module
add_subdirectory(src/io)
add_subdirectory(src/filters)


# Future modules:
# add_subdirectory(src/filters)
# add_subdirectory(src/segmentation)
# add_subdirectory(src/registration)
# add_subdirectory(src/analysis)

# ----------------------------------------------------------------------------
# Warnings (no formatting or linting here)
# ----------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ----------------------------------------------------------------------------
# Optional install rule (future packaging)
# ----------------------------------------------------------------------------
install(DIRECTORY include/ DESTINATION include)
