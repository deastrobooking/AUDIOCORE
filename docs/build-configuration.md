# Build Configuration

This guide covers CMake configuration, build options, and platform-specific settings for the LIVE-SYSTEMS framework.

## Requirements

### Minimum Requirements

- **CMake**: 3.15 or higher (3.20+ recommended)
- **C++ Compiler**: C++20 support required
  - MSVC 2019 16.11+ (Windows)
  - GCC 10+ (Linux)
  - Clang 12+ (macOS)
- **Git**: For JUCE dependency management

### Supported Platforms

- **Windows**: 10/11, Visual Studio 2019+
- **macOS**: 10.15+, Xcode 12+
- **Linux**: Ubuntu 20.04+, CentOS 8+

## Basic Configuration

### Quick Start

```bash
# Clone repository
git clone https://github.com/your-org/LIVE-SYSTEMS.git
cd LIVE-SYSTEMS

# Configure build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build framework and examples
cmake --build . -j8
```

### Generator Selection

#### Windows
```bash
# Visual Studio (recommended)
cmake .. -G "Visual Studio 17 2022" -A x64

# Ninja (faster builds)
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
```

#### macOS
```bash
# Xcode
cmake .. -G Xcode

# Ninja
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
```

#### Linux
```bash
# Make
cmake .. -G "Unix Makefiles"

# Ninja (recommended)
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
```

## CMake Options

### Framework Options

```bash
# Enable/disable components
cmake .. -DLIVE_SYSTEMS_BUILD_EXAMPLES=ON        # Build example plugins (default: ON)
cmake .. -DLIVE_SYSTEMS_BUILD_TESTS=OFF          # Build unit tests (default: OFF)
cmake .. -DLIVE_SYSTEMS_BUILD_DOCS=OFF           # Build documentation (default: OFF)

# Plugin format selection
cmake .. -DLIVE_SYSTEMS_FORMATS="VST3;AU;Standalone"  # Plugin formats to build

# Development options
cmake .. -DLIVE_SYSTEMS_ENABLE_WARNINGS=ON       # Enable compiler warnings (default: ON)
cmake .. -DLIVE_SYSTEMS_WARNINGS_AS_ERRORS=OFF   # Treat warnings as errors (default: OFF)
cmake .. -DLIVE_SYSTEMS_ENABLE_SANITIZERS=OFF    # Enable AddressSanitizer (default: OFF)
```

### JUCE Configuration

```bash
# JUCE options
cmake .. -DJUCE_BUILD_EXTRAS=OFF                 # Don't build JUCE extras
cmake .. -DJUCE_BUILD_EXAMPLES=OFF               # Don't build JUCE examples

# Copy plugins after build
cmake .. -DJUCE_COPY_PLUGIN_AFTER_BUILD=ON       # Auto-install plugins (default: ON)
```

### Compiler Options

```bash
# C++ standard
cmake .. -DCMAKE_CXX_STANDARD=20

# Build type
cmake .. -DCMAKE_BUILD_TYPE=Release              # Release, Debug, RelWithDebInfo, MinSizeRel

# Custom compiler flags
cmake .. -DCMAKE_CXX_FLAGS="-march=native"       # Enable CPU-specific optimizations
```

## Advanced Configuration

### Custom JUCE Installation

If you have JUCE installed locally:

```cmake
# In your CMakeLists.txt
set(JUCE_PATH "/path/to/your/juce" CACHE PATH "Path to JUCE")
add_subdirectory(${JUCE_PATH} juce)
```

Or via command line:

```bash
cmake .. -DJUCE_PATH=/path/to/your/juce
```

### Plugin Installation Paths

```bash
# Custom plugin installation directory
cmake .. -DCMAKE_INSTALL_PREFIX=/custom/path

# Platform-specific overrides
cmake .. -DLIVE_SYSTEMS_VST3_INSTALL_PATH="/custom/vst3/path"
cmake .. -DLIVE_SYSTEMS_AU_INSTALL_PATH="/custom/au/path"
```

### Cross-Compilation

#### Windows to ARM64

```bash
cmake .. -G "Visual Studio 17 2022" -A ARM64
```

#### macOS Universal Binary

```bash
cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
```

#### Linux Cross-Compilation

```bash
# For ARM64
cmake .. -DCMAKE_SYSTEM_NAME=Linux \
         -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
         -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
         -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
```

## Build Configurations

### Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DLIVE_SYSTEMS_ENABLE_ASSERTIONS=ON \
         -DLIVE_SYSTEMS_ENABLE_SANITIZERS=ON

# Build with debug info
cmake --build . --config Debug
```

Debug builds include:
- Debug symbols
- Runtime assertions
- Slower but safer code
- Additional logging

### Release Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DLIVE_SYSTEMS_ENABLE_LTO=ON \
         -DCMAKE_CXX_FLAGS="-march=native"

# Build optimized
cmake --build . --config Release
```

Release builds include:
- Full optimizations
- Link-time optimization (LTO)
- Minimal debug info
- CPU-specific optimizations

### Profile Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo \
         -DLIVE_SYSTEMS_ENABLE_PROFILING=ON

# Build with profiling support
cmake --build . --config RelWithDebInfo
```

## Platform-Specific Configuration

### Windows Configuration

```cmake
# Visual Studio specific
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SimpleDistortion)

# Windows-specific definitions
target_compile_definitions(MyPlugin PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0A00  # Windows 10+
)

# Enable Hot Reload (Edit and Continue)
if(MSVC)
    target_compile_options(MyPlugin PRIVATE
        $<$<CONFIG:Debug>:/ZI>
        $<$<CONFIG:Debug>:/Ob0>
    )
    target_link_options(MyPlugin PRIVATE
        $<$<CONFIG:Debug>:/INCREMENTAL>
    )
endif()
```

### macOS Configuration

```cmake
# macOS deployment target
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version")

# Code signing (for distribution)
set_target_properties(MyPlugin PROPERTIES
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Developer ID Application: Your Name"
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "TEAM_ID"
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
)

# Hardened runtime (required for notarization)
set_target_properties(MyPlugin PROPERTIES
    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
)
```

### Linux Configuration

```cmake
# Linux-specific libraries
if(LINUX)
    target_link_libraries(MyPlugin PRIVATE
        pthread
        dl
        rt
    )
    
    # Install desktop files
    install(FILES ${CMAKE_SOURCE_DIR}/assets/MyPlugin.desktop
            DESTINATION share/applications)
endif()
```

## Dependency Management

### JUCE Configuration

The framework automatically downloads and configures JUCE:

```cmake
# Automatic JUCE setup
include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(JUCE)
```

### Custom Dependencies

Add additional dependencies as needed:

```cmake
# Example: Add custom DSP library
FetchContent_Declare(
    MyDSPLib
    GIT_REPOSITORY https://github.com/example/dsp-lib.git
    GIT_TAG        v1.0.0
)

FetchContent_MakeAvailable(MyDSPLib)

target_link_libraries(LIVE-SYSTEMS-Framework PRIVATE MyDSPLib::DSP)
```

## Testing Configuration

### Unit Tests

```bash
# Enable testing
cmake .. -DLIVE_SYSTEMS_BUILD_TESTS=ON

# Build and run tests
cmake --build . --target LiveSystemsTests
ctest --output-on-failure
```

### Integration Tests

```bash
# Build example plugins for testing
cmake .. -DLIVE_SYSTEMS_BUILD_EXAMPLES=ON

# Test plugin loading
cmake --build . --target test_plugin_loading
```

## Packaging and Distribution

### CPack Configuration

```cmake
# Enable CPack
include(CPack)

set(CPACK_PACKAGE_NAME "LIVE-SYSTEMS")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Modern Audio Plugin Framework")
set(CPACK_PACKAGE_CONTACT "your-email@example.com")

# Platform-specific packaging
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()
```

Build packages:

```bash
cmake --build . --target package
```

### Installation

```bash
# Install to system directories
cmake --build . --target install

# Create installer package
cmake --build . --target package
```

## Optimization Options

### Link-Time Optimization

```cmake
# Enable LTO for release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    
    if(ipo_supported)
        set_target_properties(LIVE-SYSTEMS-Framework PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE
        )
    endif()
endif()
```

### CPU-Specific Optimizations

```cmake
# Native CPU optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(LIVE-SYSTEMS-Framework PRIVATE
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-mtune=native>
    )
elseif(MSVC)
    target_compile_options(LIVE-SYSTEMS-Framework PRIVATE
        $<$<CONFIG:Release>:/arch:AVX2>
    )
endif()
```

### Memory Optimizations

```cmake
# Reduce binary size
if(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_options(LIVE-SYSTEMS-Framework PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Os>
        $<$<CXX_COMPILER_ID:Clang>:-Oz>
        $<$<CXX_COMPILER_ID:MSVC>:/Os>
    )
    
    target_link_options(LIVE-SYSTEMS-Framework PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-s>
        $<$<CXX_COMPILER_ID:Clang>:-s>
    )
endif()
```

## Troubleshooting

### Common Issues

#### CMake Version Too Old
```
CMake Error: CMake 3.15 or higher is required.
```
**Solution**: Update CMake to version 3.15 or higher.

#### C++20 Not Supported
```
error: C++20 is not supported by this compiler
```
**Solution**: Update your compiler or use a compatible version.

#### JUCE Download Failed
```
FetchContent failed to download JUCE
```
**Solution**: Check internet connection or use local JUCE installation.

#### Plugin Not Loading
```
Plugin failed to load in host
```
**Solution**: Check plugin format compatibility and dependencies.

### Debug Build Issues

Enable verbose output:

```bash
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build . --verbose
```

Check compiler flags:

```bash
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

### Performance Issues

Profile build times:

```bash
# Ninja build with timing
cmake .. -G Ninja
ninja -j1 -v -d stats

# Visual Studio with timing
cmake --build . --config Release -- /verbosity:diagnostic
```

## Continuous Integration

### GitHub Actions Example

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [windows-latest, macos-latest, ubuntu-latest]
        
    runs-on: ${{ matrix.os }}
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      
    - name: Build
      run: cmake --build build --config Release
      
    - name: Test
      run: ctest --test-dir build --config Release
```

This comprehensive build configuration guide covers all aspects of building and configuring the LIVE-SYSTEMS framework for development and distribution.