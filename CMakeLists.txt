# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.15)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Set project name and version
project(LIVE-SYSTEMS VERSION 1.0.0)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Visual Studio specific settings
if(MSVC)
    # Use folder organization in VS solution
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    
    # Configure Visual Studio debugging
    set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD ON)
    
    # MSVC compiler flags
    add_compile_options(
        /W4                 # Warning level 4
        /MP                 # Multiprocessor compilation
        /permissive-        # Standards conformance mode
        /EHsc               # Exception handling model
        /wd4100             # Unreferenced formal parameter (common with callbacks)
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang warnings
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Project options
option(LIVE_SYSTEMS_BUILD_TESTS "Build the test suite" OFF)

# Add JUCE as a submodule or external dependency
# We'll use FetchContent to automatically download JUCE
include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0  # Use latest stable version
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(JUCE)

# Enable testing if requested
if(LIVE_SYSTEMS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Include sub-projects
add_subdirectory("LIVE-SYSTEMS")

# Add plugin projects
add_subdirectory("plugins")
