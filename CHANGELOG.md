# Changelog

All notable changes to the AUDIOCORE LIVE-SYSTEMS framework will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- Comprehensive documentation suite
- Project status tracking
- Build status badges

---

## [1.0.0] - 2025-10-06

### 🎉 Major Release - Production Ready

This release marks the first production-ready version of the AUDIOCORE LIVE-SYSTEMS framework with full multi-platform support and comprehensive CI/CD integration.

### Added

#### Build System
- ✅ **Position Independent Code (PIC)** support for VST3 plugin builds
  - Global `CMAKE_POSITION_INDEPENDENT_CODE` setting
  - Fixes Linux/macOS shared library linking
  - Required for VST3 format
  
- ✅ **macOS Build Support** in GitHub Actions
  - Added `macos-latest` to build matrix
  - Clang compiler configuration
  - Homebrew dependency installation (Ninja)
  - Audio Unit (AU) plugin format support
  - Universal binary support (ARM64 + x86_64)

- ✅ **Visual Studio / MSVC Optimizations**
  - Warning level 4 (`/W4`)
  - Multiprocessor compilation (`/MP`)
  - Standards conformance mode (`/permissive-`)
  - Proper exception handling (`/EHsc`)
  - Folder organization in Solution Explorer
  - Hot Reload support (Edit and Continue)

- ✅ **CMake Presets Enhancement**
  - Added build presets for all platforms
  - Added test presets for automated testing
  - Improved Visual Studio integration

#### Dependencies
- ✅ **libcurl4-openssl-dev** for Linux builds
  - Resolves JUCE dependency requirements
  - Fixes compilation errors on Ubuntu

#### Documentation
- ✅ **Visual Studio Review** (`docs/visual-studio-review.md`)
  - Complete MSVC compatibility analysis
  - Code quality assessment
  - Testing checklist
  - Best practices guide

- ✅ **PIC Fix Summary** (`docs/pic-fix-summary.md`)
  - Technical explanation of Position Independent Code
  - Shared library linking details
  - Platform-specific considerations

- ✅ **macOS Build Support** (`docs/macos-build-support.md`)
  - Comprehensive macOS build guide
  - Audio Unit configuration
  - Universal binary creation
  - Testing procedures

- ✅ **Project Status Report** (`docs/PROJECT-STATUS.md`)
  - Current build status
  - Feature implementation tracking
  - Roadmap and milestones

- ✅ **.editorconfig** file
  - Consistent code formatting across IDEs
  - Platform-specific line endings
  - Language-specific indentation

#### Framework Components
- ✅ **LiveSystemsExport.h**
  - DLL export/import macros for Windows
  - Future-proofing for dynamic library builds
  - MSVC-specific warning suppression

### Fixed

#### Build Errors
- ✅ **Linux VST3 Linker Error**
  - Issue: `relocation R_X86_64_TPOFF32 can not be used when making a shared object`
  - Solution: Enabled Position Independent Code globally
  - Status: All Linux builds now passing

- ✅ **Missing curl/curl.h Header**
  - Issue: `fatal error: 'curl/curl.h' file not found`
  - Solution: Added libcurl4-openssl-dev to Linux dependencies
  - Status: Resolved in GitHub Actions workflow

- ✅ **macOS Builds Not Triggered**
  - Issue: macOS missing from build matrix
  - Solution: Added macos-latest with proper configuration
  - Status: macOS builds now running with AU support

### Changed

#### Build Configuration
- Updated CMakeLists.txt with platform-specific optimizations
- Enhanced GitHub Actions workflow for multi-platform support
- Improved cache strategy for faster CI/CD builds

#### Compiler Flags
- Added strict warning levels for all platforms
- Enabled conformance mode on MSVC
- Added GCC/Clang warning flags (`-Wall -Wextra -Wpedantic`)

### Security
- ✅ Strict pointer checking enabled (`JUCE_STRICT_REFCOUNTEDPOINTER`)
- ✅ Memory leak detection active (JUCE leak detector)
- ✅ Standards conformance mode prevents undefined behavior

---

## Build Status History

### 2025-10-06: All Tests Passing ✅

| Platform | Compiler | Status | Details |
|----------|----------|--------|---------|
| Linux (Ubuntu 22.04) | GCC | ✅ PASSING | Release + Debug |
| Linux (Ubuntu 22.04) | Clang | ✅ PASSING | Release + Debug |
| Windows (Latest) | MSVC | ✅ PASSING | Release + Debug |
| macOS (Latest) | Clang | ✅ PASSING | Release + Debug |

**Total:** 8/8 builds passing (100% success rate)

**Plugin Formats Built:**
- ✅ VST3 (All platforms)
- ✅ AU - Audio Unit (macOS)
- ✅ Standalone (All platforms)

---

## Technical Details

### Compilers Tested
- **GCC**: 10.0+ (Linux)
- **Clang**: 12.0+ (Linux, macOS)
- **MSVC**: 19.29+ (Visual Studio 2019+)
- **Apple Clang**: Latest (Xcode Command Line Tools)

### Platforms Tested
- **Linux**: Ubuntu 22.04 LTS
- **Windows**: Windows Server 2022 (GitHub Actions)
- **macOS**: macOS 14 Sonoma (GitHub Actions)

### Architectures
- **x86_64**: Intel/AMD 64-bit
- **ARM64**: Apple Silicon
- **Universal Binary**: ARM64 + x86_64 (macOS)

### Dependencies
- **JUCE**: 8.0.0
- **CMake**: 3.15+ (tested with 3.31)
- **Ninja**: Latest stable

---

## Migration Guide

### For Existing Projects

If you're updating from an earlier version, no code changes are required. The build system improvements are backward compatible.

### Recommended Actions
1. Pull latest changes
2. Clean build directory
3. Reconfigure with CMake
4. Rebuild project

```bash
rm -rf build/
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## Known Issues

None. All platforms building successfully.

---

## Deprecations

None in this release.

---

## Contributors

- **deastrobooking** - Project lead and maintainer
- **GitHub Copilot** - AI-assisted development and code review

---

## Links

- **Repository**: https://github.com/deastrobooking/AUDIOCORE
- **Issues**: https://github.com/deastrobooking/AUDIOCORE/issues
- **Documentation**: `/docs` directory
- **JUCE**: https://juce.com

---

## Release Notes Summary

### What's New in v1.0.0

🎉 **Production-Ready Release**

This release represents a major milestone with full multi-platform support and comprehensive CI/CD integration. The framework is now ready for professional audio plugin development.

**Highlights:**
- ✅ 100% build success rate across all platforms
- ✅ Full VST3, AU, and Standalone support
- ✅ Universal binary support for macOS (ARM64 + x86_64)
- ✅ Professional build configuration
- ✅ Comprehensive documentation
- ✅ Working example plugin

**Platform Support:**
- Linux (GCC and Clang)
- Windows (MSVC)
- macOS (Clang) with Audio Unit support

**Ready for:**
- VST3 plugin development
- Audio Unit (AU) development (macOS)
- Standalone application development
- Cross-platform audio software

---

**Download:** See [Releases](https://github.com/deastrobooking/AUDIOCORE/releases) page

**Full Documentation:** See [docs/](docs/) directory

---

*Last Updated: October 6, 2025*
