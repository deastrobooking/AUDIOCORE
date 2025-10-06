# Frequently Asked Questions (FAQ)

## General Questions

### What is LIVE-SYSTEMS?

LIVE-SYSTEMS is a modern C++20 framework for developing professional audio plugins. Built on top of JUCE, it provides a higher-level abstraction that simplifies common plugin development tasks while maintaining performance and flexibility.

### Why use LIVE-SYSTEMS instead of pure JUCE?

LIVE-SYSTEMS provides:
- **Simplified Parameter Management**: Automatic parameter creation, state handling, and GUI binding
- **Built-in Audio Processing**: Ready-to-use filters, compressors, and reverbs
- **Preset System**: Comprehensive preset management with categories and file handling
- **Modern GUI Framework**: Themed, responsive interfaces with minimal boilerplate
- **Best Practices**: Architecture that encourages good plugin design patterns

### What plugin formats are supported?

- **VST3**: Full support on all platforms
- **AU (Audio Units)**: macOS only
- **Standalone**: Desktop applications on all platforms
- **AAX**: Supported if you have the AAX SDK (Pro Tools)

### What platforms are supported?

- **Windows**: 10/11 with Visual Studio 2019+
- **macOS**: 10.15+ with Xcode 12+
- **Linux**: Ubuntu 20.04+, CentOS 8+ with GCC 10+

## Getting Started

### How do I create my first plugin?

1. Follow the [Creating Plugins](creating-plugins.md) guide
2. Study the SimpleDistortion example
3. Use the plugin template as a starting point
4. Join our community for help and discussions

### Do I need to know JUCE to use LIVE-SYSTEMS?

Basic JUCE knowledge is helpful but not required. LIVE-SYSTEMS abstracts many JUCE complexities, but understanding JUCE concepts like:
- `AudioProcessor` and `AudioProcessorEditor`
- `Graphics` and `Component`
- Basic audio processing concepts

will make development easier.

### What C++ knowledge do I need?

You should be comfortable with:
- Modern C++ (C++17/20 features)
- Object-oriented programming
- Templates and smart pointers
- Basic audio programming concepts

## Development Questions

### How do I add parameters to my plugin?

```cpp
void MyPlugin::initializeParameters()
{
    ParameterManager::ParameterInfo gainInfo;
    gainInfo.id = "gain";
    gainInfo.name = "Gain";
    gainInfo.defaultValue = 0.0f;
    gainInfo.minValue = -24.0f;
    gainInfo.maxValue = 24.0f;
    gainInfo.units = "dB";
    
    parameterManager->addFloatParameter(gainInfo);
}
```

See the [Parameter Management](parameter-management.md) guide for details.

### How do I process audio in my plugin?

Override the `processAudio` method:

```cpp
void MyPlugin::processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    float gain = parameterManager->getParameterValue("gain");
    float linearGain = juce::Decibels::decibelsToGain(gain);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            data[sample] *= linearGain;
        }
    }
}
```

### How do I create a custom GUI?

Create an editor class inheriting from `LiveSystemsEditor`:

```cpp
class MyPluginEditor : public LiveSystemsEditor
{
protected:
    void setupComponents() override
    {
        // Add your GUI components
        addAndMakeVisible(gainSlider);
    }
    
    void paintBackground(juce::Graphics& g) override
    {
        // Custom background painting
        g.fillAll(currentTheme.backgroundColour);
    }
    
    void layoutComponents() override
    {
        // Arrange your components
        gainSlider.setBounds(10, 10, 100, 100);
    }
};
```

### How do I add presets to my plugin?

Presets are handled automatically by the framework. To add factory presets:

```cpp
void MyPlugin::initializeParameters()
{
    // ... create parameters ...
    
    // Add factory presets
    juce::ValueTree cleanState("Clean");
    cleanState.setProperty("gain", 0.0f, nullptr);
    
    presetManager->addFactoryPreset("Clean", "Factory", cleanState);
}
```

## Technical Questions

### Can I use my own DSP code with LIVE-SYSTEMS?

Yes! You can:
1. Add custom processing in `processAudio()`
2. Create custom processor classes for the audio engine
3. Use any external DSP libraries
4. Mix framework features with custom code

### How do I optimize my plugin's performance?

- Cache parameter values instead of querying every sample
- Use the built-in audio engine for common effects
- Avoid memory allocation in the audio thread
- Use SIMD instructions for intensive processing
- Profile your code regularly

See the [Performance Optimization](performance-optimization.md) guide.

### Can I distribute plugins commercially?

Yes! LIVE-SYSTEMS uses the MIT license, which allows commercial use. However:
- Check JUCE licensing for your use case
- Ensure you have proper licenses for any external libraries
- Follow platform-specific distribution requirements

### How do I debug audio issues?

1. Use the standalone version for easier debugging
2. Add logging to track parameter changes
3. Use JUCE's built-in debugging tools
4. Profile audio performance
5. Test with different buffer sizes and sample rates

## Build and Configuration

### Why is my build failing?

Common issues:
- **CMake too old**: Update to 3.15+
- **Compiler doesn't support C++20**: Update compiler
- **Missing dependencies**: Ensure all requirements are met
- **JUCE download failed**: Check internet connection

See [Build Configuration](build-configuration.md) for detailed troubleshooting.

### How do I build for different platforms?

The framework supports cross-compilation:

```bash
# Windows ARM64
cmake .. -G "Visual Studio 17 2022" -A ARM64

# macOS Universal Binary
cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"

# Linux ARM64
cmake .. -DCMAKE_SYSTEM_PROCESSOR=aarch64
```

### Can I use a different build system than CMake?

While CMake is recommended and fully supported, you can adapt the framework to other build systems. You'll need to:
- Link with JUCE properly
- Set up the correct include paths
- Configure compiler flags for C++20
- Handle platform-specific settings

## Common Issues

### My plugin isn't loading in the host

Check:
1. Plugin format compatibility (VST3 vs AU vs AAX)
2. Architecture match (x64 vs ARM)
3. Dependencies are available
4. Plugin is in the correct directory
5. No missing DLLs/frameworks

### GUI isn't updating when parameters change

Ensure you're:
1. Connecting sliders to parameters properly
2. Using parameter listeners for real-time updates
3. Calling `repaint()` when needed
4. Not blocking the message thread

### Audio is crackling or glitching

Common causes:
1. Memory allocation in audio thread
2. Blocking operations in `processAudio()`
3. Buffer underruns
4. Incorrect sample rate handling
5. Thread synchronization issues

### Parameters aren't being saved/recalled

Check:
1. Parameter IDs are consistent
2. State serialization is working
3. Host supports parameter automation
4. Preset files aren't corrupted

## Community and Support

### Where can I get help?

- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For questions and community help
- **Documentation**: Comprehensive guides and references
- **Examples**: Study the provided example plugins

### How can I contribute?

See our [Contributing Guide](contributing.md) for:
- Code contributions
- Documentation improvements
- Bug reports
- Feature requests
- Community support

### Is there a community forum?

We use GitHub Discussions as our main community forum. You can:
- Ask questions
- Share your plugins
- Discuss best practices
- Request features
- Help other developers

### How do I report a bug?

1. Check existing issues first
2. Create a minimal reproduction case
3. Include system information
4. Provide build logs if relevant
5. Describe expected vs actual behavior

### Can I request new features?

Absolutely! We welcome feature requests:
1. Check the roadmap first
2. Open a GitHub issue with the "enhancement" label
3. Describe the use case and benefits
4. Consider contributing the implementation

---

**Still have questions?**

- Browse the complete [documentation](README.md)
- Check [GitHub Issues](https://github.com/your-org/LIVE-SYSTEMS/issues)
- Start a [Discussion](https://github.com/your-org/LIVE-SYSTEMS/discussions)
- Look at the example plugins for inspiration