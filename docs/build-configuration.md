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
    GIT_TAG        8.0.4  # Updated for macOS 15 compatibility
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
This is the most effective way to solve the "binary incompatibility" problem. Since you cannot convert the files directly, you must create a single codebase that can be **compiled** three separate times to create the native files for each OS.

Here is a comprehensive guide to building cross-platform audio plugins.

-----

### **The Golden Rule: "Write Once, Compile Everywhere"**

To avoid writing three separate programs (one for Windows, one for Mac, one for Linux), you need a **Cross-Platform Framework**. This framework handles the "boring" stuff—creating windows, handling mouse clicks, and talking to the OS—so you can focus on the Digital Signal Processing (DSP).

### **1. Choose Your Framework**

There are three main industry standards for this.

| Framework | Best For... | Pros | Cons |
| :--- | :--- | :--- | :--- |
| **JUCE** | **The Industry Standard** | Extremely robust, huge community, supports all formats (VST3, AU, LV2, CLAP), works on Linux/Win/Mac/iOS/Android. | Licensed (free for personal/GPL, paid for closed-source commercial revenue \>$50k). |
| **iPlug2** | Lightweight Plugins | Lighter than JUCE, great for simple graphics. | Smaller community, documentation can be sparse. |
| **DISTRHO (DPF)** | **Linux/Open Source** | Excellent Linux support, strict focus on VST2/VST3/LV2. | Less "hand-holding" than JUCE. |

> **Recommendation:** Start with **JUCE**. It is the most widely used and has the best documentation for beginners.

-----

### **2. The Development Workflow**

Instead of trying to move a `.dll` to Linux, your workflow looks like this:

1.  **Shared Source Code (.cpp/.h):** This contains your math (DSP) and your UI logic.
2.  **Build System (CMake):** A script that tells the compiler how to handle the files for the specific OS.
3.  **The Compilers:**
      * **Windows Machine:** Uses MSVC (Visual Studio) $\rightarrow$ Outputs `.vst3` (dll based).
      * **Mac Machine:** Uses Clang (Xcode) $\rightarrow$ Outputs `.component` (bundle).
      * **Linux Machine:** Uses GCC/Clang $\rightarrow$ Outputs `.so` (VST3/LV2).

-----

### **3. Setting Up Your Environment**

You will need a specific setup for each OS you intend to build on.

#### **A. Windows Setup**

  * **IDE:** Visual Studio Community (Free).
  * **Required Components:** "Desktop development with C++".
  * **Output:** `.vst3` (which is technically a DLL folder structure).

#### **B. macOS Setup**

  * **IDE:** Xcode (Available on App Store).
  * **Required Components:** Command Line Tools.
  * **Output:** `.component` (Audio Unit) and `.vst3` (Bundle).

#### **C. Linux Setup**

  * **IDE:** VS Code or CLion.
  * **Required Tools:** GCC, G++, Make, Ninja, and development libraries (specifically X11, ALSA, JACK, and Freetype).
  * **Output:** `.vst3` (Shared Object) or `.lv2` folder.

-----

### **4. A Real-World Example: Using JUCE & CMake**

The modern standard for building plugins is using **CMake**. This allows you to write one configuration file that works on all three operating systems.

**The Project Structure:**

```text
MyPlugin/
├── CMakeLists.txt       <-- The "Recipe"
├── Source/
│   ├── PluginProcessor.cpp  <-- The Audio/Math code
│   ├── PluginProcessor.h
│   ├── PluginEditor.cpp     <-- The GUI code
│   └── PluginEditor.h
└── JUCE/                <-- The Framework Library
```

**The `CMakeLists.txt` (Simplified):**

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyDistortionPlugin VERSION 1.0.0)

# Add JUCE (The Framework)
add_subdirectory(JUCE)

# Define the Plugin
juce_add_plugin(MyDistortionPlugin
    COMPANY_NAME "MyAudioCo"
    IS_SYNTH FALSE
    FORMATS VST3 AU Standalone  # Build these formats
    PRODUCT_NAME "SuperDistort"
)

# Link the source code
target_sources(MyDistortionPlugin PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

# Link libraries (Linux needs extra help here usually)
target_link_libraries(MyDistortionPlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
```

**How to Build:**

1.  **On Windows:** Open command prompt $\rightarrow$ `cmake -B build` $\rightarrow$ Open the generated Visual Studio solution $\rightarrow$ Click "Build".
2.  **On Linux:** Terminal $\rightarrow$ `cmake -B build` $\rightarrow$ `cmake --build build`.
3.  **On Mac:** Terminal $\rightarrow$ `cmake -B build -G Xcode` $\rightarrow$ Open Xcode project $\rightarrow$ Click "Build".

-----

### **5. The "I Don't Own 3 Computers" Problem**

Most independent developers do not own a Windows PC, a Linux Desktop, and a Mac.

**The Solution: GitHub Actions (CI/CD)**
You can host your code on GitHub and use "Actions" (cloud computers) to compile the code for you.

1.  You push your C++ code to GitHub.
2.  GitHub spins up a virtual Windows machine, a virtual Mac, and a virtual Linux machine.
3.  They all compile the plugin simultaneously.
4.  You download a `.zip` file containing the `.dll`, `.vst3`, and `.component` ready for distribution.

### **6. Handling OS Differences in Code**

Sometimes, you *do* need to write specific code (e.g., Windows uses `MAX_PATH`, Linux uses `PATH_MAX`). You handle this with **Preprocessor Directives**:

```cpp
void openFileDialog()
{
    #if JUCE_WINDOWS
        // Windows specific file opening code (or let JUCE handle it)
        WindowsAPI::OpenDialog(...);
    #elif JUCE_MAC
        // Mac specific code
        CocoaAPI::OpenDialog(...);
    #elif JUCE_LINUX
        // Linux specific code
        LinuxGTK::OpenDialog(...);
    #endif
}
```

### **Summary of Output Files**

| Platform | VST3 Format | Audio Unit (AU) | LV2 | CLAP |
| :--- | :--- | :--- | :--- | :--- |
| **Windows** | `Plugin.vst3` (contains .dll) | N/A | `Plugin.lv2` (dll inside) | `Plugin.clap` (dll based) |
| **Linux** | `Plugin.vst3` (contains .so) | N/A | `Plugin.lv2` (so inside) | `Plugin.clap` (so based) |
| **macOS** | `Plugin.vst3` (Bundle) | `Plugin.component` | `Plugin.lv2` (dylib inside) | `Plugin.clap` (Bundle) |
