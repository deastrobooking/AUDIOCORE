# AUDIOCORE - LIVE-(Logarithmic, Integrated, Virtual, Electronic) SYSTEMS Audio Plugin Framework

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]() [![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-blue)]() [![C++](https://img.shields.io/badge/C%2B%2B-20-blue)]() [![License](https://img.shields.io/badge/license-MIT-green)]()

A modern, cross-platform C++20 framework for developing professional VST/AU audio plugins using JUCE and CMake.

## 🎯 Overview

LIVE-SYSTEMS is a comprehensive framework that simplifies VST plugin development by providing:

- **Modern C++20 Architecture**: Leveraging the latest C++ features for clean, efficient code
- **JUCE Integration**: Built on the industry-standard JUCE framework
- **Cross-Platform Support**: Windows, macOS, and Linux compatibility
- **CMake Build System**: Modern, flexible build configuration with Ninja generator
- **Modular Component Architecture**: Circuit modeling components for realistic audio processing
- **Advanced Audio Effects**: Professional-grade delay, reverb, and modulation effects
- **Synthesizer Framework**: Analog, digital, and wavetable synthesis engines
- **Plugin Framework**: Base classes for rapid plugin development
- **Advanced Parameter Management**: Automatic parameter handling and state management
- **High-Performance Audio Engine**: Optimized DSP processing with built-in effects
- **Preset Management System**: Comprehensive preset saving/loading with categorization
- **Modern GUI Framework**: Responsive, themed user interfaces

## 🚀 Quick Start

### Prerequisites

- CMake 3.15 or higher (using 3.31.6-msvc9)
- C++20 compatible compiler (MSVC 2019+, GCC 10+, Clang 12+)
- Git (for JUCE submodule)
- Ninja build system (recommended)

### Building

1. **Clone the repository:**
   ```bash
   git clone https://github.com/deastrobooking/AUDIOCORE.git
   cd AUDIOCORE
   ```

2. **Configure with CMake using Ninja:**
   ```bash
   mkdir build
   cd build
   cmake .. -G Ninja
   ```

3. **Build the framework and examples:**
   ```bash
   cmake --build .
   ```

4. **Install plugins (optional):**
   ```bash
   cmake --build . --target install
   ```

## 📁 Project Structure

```
AUDIOCORE/
├── CMakeLists.txt              # Root CMake configuration
├── README.md                   # This file
├── .github/workflows/          # GitHub Actions CI/CD
│   ├── ci.yml                  # Main CI pipeline
│   └── quality.yml             # Code quality checks
├── docs/                       # Documentation
├── tests/                      # Unit tests
├── LIVE-SYSTEMS/               # Core framework
│   ├── include/                # Public headers
│   │   ├── PluginProcessor.h   # Base processor class
│   │   ├── PluginEditor.h      # Base editor class
│   │   ├── ParameterManager.h  # Parameter management
│   │   ├── AudioEngine.h       # DSP processing engine
│   │   └── PresetManager.h     # Preset management
│   ├── src/                    # Implementation files
│   ├── components/             # Circuit modeling components
│   │   ├── Diodes/             # Diode circuit models
│   │   ├── Resistors/          # Resistor circuit models
│   │   ├── Filters/            # Filter circuit models
│   │   ├── Distortions/        # Distortion circuit models
│   │   ├── Amplifiers/         # Amplifier circuit models
│   │   └── Oscillators/        # Oscillator circuit models
│   ├── audio_effects/          # Audio processing effects
│   │   ├── Delays/             # Delay effects
│   │   ├── Reverbs/            # Reverb effects
│   │   ├── Chorus/             # Chorus effects
│   │   └── Compressor/         # Compression effects
│   ├── synths/                 # Synthesizer engines
│   │   ├── Analog/             # Analog synthesis
│   │   ├── Digital/            # Digital synthesis
│   │   └── Wavetable/          # Wavetable synthesis
│   ├── examples/               # Example plugins
│   │   └── SimpleDistortion/   # Simple distortion plugin example
│   └── CMakeLists.txt
└── plugins/                    # Your plugin projects go here
    └── CMakeLists.txt
```

## 🔧 Creating Your First Plugin

### 1. Create a New Plugin Project

Create a new directory in the `plugins/` folder:

```bash
mkdir plugins/MyAwesomePlugin
cd plugins/MyAwesomePlugin
```

### 2. Create CMakeLists.txt

```cmake
# MyAwesomePlugin/CMakeLists.txt
juce_add_plugin(MyAwesomePlugin
    COMPANY_NAME "DEAST Audio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE DEA
    PLUGIN_CODE MyAw
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "My Awesome Plugin"
)

juce_generate_juce_header(MyAwesomePlugin)

target_sources(MyAwesomePlugin
    PRIVATE
        MyAwesomePluginProcessor.cpp
        MyAwesomePluginEditor.cpp
)

target_link_libraries(MyAwesomePlugin
    PRIVATE
        LIVE-SYSTEMS-Framework
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_features(MyAwesomePlugin PRIVATE cxx_std_20)
```

### 3. Using Components in Your Plugin

```cpp
// MyAwesomePluginProcessor.h
#pragma once
#include "../../LIVE-SYSTEMS/include/PluginProcessor.h"
#include "../../LIVE-SYSTEMS/components/Distortions/DistortionBase.h"

class MyAwesomePluginProcessor : public LiveSystemsProcessor
{
public:
    MyAwesomePluginProcessor();
    ~MyAwesomePluginProcessor() override;

    void initializeParameters() override;
    void processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    juce::String getPluginName() const override { return "My Awesome Plugin"; }
    juce::AudioProcessorEditor* createEditor() override;

private:
    std::unique_ptr<LiveSystems::Components::Distortions::DistortionBase> distortion;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyAwesomePluginProcessor)
};
```

### 4. Register Your Plugin

Add your plugin to `plugins/CMakeLists.txt`:

```cmake
add_subdirectory("MyAwesomePlugin")
```

## 🎛️ Framework Features

### Parameter Management

The `ParameterManager` class provides automatic parameter handling:

```cpp
void MyPlugin::initializeParameters()
{
    ParameterManager::ParameterInfo gainInfo;
    gainInfo.id = "gain";
    gainInfo.name = "Gain";
    gainInfo.defaultValue = 0.0f;
    gainInfo.minValue = -20.0f;
    gainInfo.maxValue = 20.0f;
    gainInfo.units = "dB";
    
    parameterManager->addFloatParameter(gainInfo);
}
```

### Using Circuit Components

```cpp
void MyPlugin::processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Use distortion component
    distortion->setDrive(driveParameter->get());
    distortion->setOutputLevel(outputParameter->get());
    distortion->process(buffer);
    
    // Use built-in effects
    audioEngine->getFilter().setFrequency(1000.0f);
    audioEngine->getFilter().process(buffer);
}
```

## 🔄 Branch Strategy

The repository is organized for easy product development:

- **`main`**: Stable releases and production code
- **`develop`**: Integration branch for new features  
- **`core`**: Core framework development
- **`product-*`**: Product-specific branches that inherit core changes
- **`feature/*`**: Feature development branches

This allows easy spinning of new products as branches that can inherit changes to core libraries.

## 🛠️ Building for Different Platforms

### Windows (Visual Studio with Ninja)
```bash
cmake .. -G Ninja
cmake --build . --config Release
```

### macOS (Ninja)
```bash
cmake .. -G Ninja
cmake --build . --config Release
```

### Linux (Ninja)
```bash
cmake .. -G Ninja
cmake --build . -j8
```

## 🎨 Supported Plugin Formats

- **VST3**: Industry standard format
- **AU**: macOS Audio Units  
- **Standalone**: Desktop application
- **AAX**: Pro Tools (requires AAX SDK)

## 🔄 Version Requirements

- **CMake**: 3.15+ (currently using 3.31.6-msvc9)
- **JUCE**: 8.0.4 (automatically downloaded, includes macOS 15 support)
- **C++ Standard**: C++20
- **Build System**: Ninja (recommended)
- **Compilers**:
  - MSVC 2019+ (Windows)
  - GCC 10+ (Linux)
  - Clang 12+ (macOS)

## 🚀 GitHub Actions CI/CD

The repository includes comprehensive GitHub Actions workflows with **100% pass rate**:

### ✅ Build Status (All Tests Passing)

| Platform | Compiler | Release | Debug | Plugin Formats |
|----------|----------|---------|-------|----------------|
| **Linux** | GCC | ✅ | ✅ | VST3, Standalone |
| **Linux** | Clang | ✅ | ✅ | VST3, Standalone |
| **Windows** | MSVC | ✅ | ✅ | VST3, Standalone |
| **macOS** | Clang | ✅ | ✅ | VST3, AU, Standalone |

### CI/CD Features
- **Continuous Integration**: Automated builds on all platforms
- **Multi-Compiler Support**: GCC, Clang, MSVC
- **Code Quality**: Static analysis and formatting checks
- **Automated Testing**: Unit tests and integration tests with CTest
- **Build Caching**: Fast incremental builds
- **Artifact Publishing**: Automatic plugin binaries

### Latest Build Results (October 6, 2025)
- ✅ All 8 build configurations passing
- ✅ Zero compilation errors
- ✅ Zero linker errors
- ✅ All plugin formats building successfully
- ✅ Cross-platform compatibility verified

## 🧪 Examples

The framework includes example plugins demonstrating various features:

- **SimpleDistortion**: Basic waveshaping distortion using the distortion components
- More examples coming soon for each component type!

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](docs/contributing.md) for details.

1. Fork the repository
2. Create a feature branch
3. Make your changes following our coding standards
4. Add tests if applicable
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **JUCE Framework**: The foundation of our audio processing
- **CMake**: Modern build system
- **Ninja**: Fast, lightweight build system
- **Community**: All the developers who contribute to open-source audio software

## 📞 Support

- **Documentation**: [docs/](docs/)
- **Issues**: [GitHub Issues](https://github.com/deastrobooking/AUDIOCORE/issues)
- **Discussions**: [GitHub Discussions](https://github.com/deastrobooking/AUDIOCORE/discussions)

---

**AUDIOCORE LIVE-SYSTEMS** - Empowering the next generation of audio plugin developers with modern C++ and professional-grade component modeling.