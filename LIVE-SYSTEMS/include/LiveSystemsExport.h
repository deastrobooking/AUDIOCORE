#pragma once

/**
 * @file LiveSystemsExport.h
 * @brief Export/Import definitions for LIVE-SYSTEMS framework
 * 
 * This header defines macros for exporting/importing symbols when building
 * the framework as a DLL on Windows. Currently the framework is built as a
 * static library, but these macros are provided for future DLL support.
 */

// Export/Import definitions for Windows DLL builds
#ifdef _WIN32
    #ifdef LIVE_SYSTEMS_BUILD_DLL
        // Building the DLL - export symbols
        #define LIVE_SYSTEMS_API __declspec(dllexport)
    #elif defined(LIVE_SYSTEMS_USE_DLL)
        // Using the DLL - import symbols
        #define LIVE_SYSTEMS_API __declspec(dllimport)
    #else
        // Static library build - no special decoration needed
        #define LIVE_SYSTEMS_API
    #endif
#else
    // Non-Windows platforms - no special decoration needed
    #define LIVE_SYSTEMS_API
#endif

// Disable specific MSVC warnings for the framework
#ifdef _MSC_VER
    // C4251: class 'X' needs to have dll-interface to be used by clients of class 'Y'
    // This is safe to ignore when using JUCE containers
    #pragma warning(disable: 4251)
    
    // C4275: non dll-interface class 'X' used as base for dll-interface class 'Y'
    // This is safe to ignore when inheriting from JUCE classes
    #pragma warning(disable: 4275)
#endif
