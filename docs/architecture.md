# Framework Architecture

## Overview

The LIVE-SYSTEMS framework is built on a modular architecture that separates concerns and provides a clean, extensible foundation for audio plugin development.

## Core Components

### 1. LiveSystemsProcessor (Base Processor)

The `LiveSystemsProcessor` class serves as the foundation for all plugins in the framework. It inherits from JUCE's `AudioProcessor` and provides:

- **Parameter Management**: Automatic parameter registration and state handling
- **Audio Engine Integration**: High-performance DSP processing
- **Preset Management**: Built-in preset system
- **Template Methods**: Virtual methods for plugin-specific implementation

```cpp
class LiveSystemsProcessor : public juce::AudioProcessor
{
    // Framework provides infrastructure
    // Derived classes implement specific behavior
};
```

### 2. LiveSystemsEditor (Base Editor)

The `LiveSystemsEditor` class provides a modern GUI framework with:

- **Theme System**: Consistent visual styling across plugins
- **Component Management**: Automatic layout and updates
- **Parameter Binding**: Easy connection between GUI and parameters
- **Responsive Design**: Adaptive layouts for different screen sizes

### 3. ParameterManager

Centralizes all parameter-related functionality:

- **Type-Safe Parameters**: Float, Choice, and Boolean parameter types
- **Automatic Serialization**: State saving/loading without boilerplate
- **Parameter Groups**: Logical organization of related parameters
- **Listener System**: Callbacks for parameter changes

### 4. AudioEngine

High-performance audio processing engine:

- **Built-in Effects**: Filter, Compressor, Reverb ready to use
- **Processor Chain**: Custom effect chaining system
- **Performance Monitoring**: CPU usage tracking
- **DSP Utilities**: Common audio processing functions

### 5. PresetManager

Comprehensive preset system:

- **Factory Presets**: Built-in presets for plugins
- **User Presets**: Custom preset creation and management
- **Categories**: Organized preset browsing
- **Import/Export**: Preset sharing capabilities

## Architecture Diagram

```
???????????????????????????????????????????????????????????????
?                      Your Plugin                           ?
???????????????????????????????????????????????????????????????
?  MyPluginProcessor    ?  MyPluginEditor                    ?
?  (inherits from)      ?  (inherits from)                   ?
?  LiveSystemsProcessor ?  LiveSystemsEditor                 ?
???????????????????????????????????????????????????????????????
?                    LIVE-SYSTEMS Framework                  ?
???????????????????????????????????????????????????????????????
?  ParameterManager ? AudioEngine ? PresetManager             ?
???????????????????????????????????????????????????????????????
?                      JUCE Framework                        ?
???????????????????????????????????????????????????????????????
?                    Operating System                        ?
???????????????????????????????????????????????????????????????
```

## Design Patterns

### Template Method Pattern

The framework uses the Template Method pattern extensively:

```cpp
class LiveSystemsProcessor
{
public:
    // Template method - framework controls the flow
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) final
    {
        // Framework processing
        audioEngine->process(buffer);
        
        // Call derived class implementation
        processAudio(buffer, midiMessages);
    }

protected:
    // Hook method - implemented by derived classes
    virtual void processAudio(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) = 0;
};
```

### Strategy Pattern

The AudioEngine uses strategies for different processing types:

```cpp
class AudioEngine
{
    std::vector<std::unique_ptr<ProcessorChain>> processorChain;
    
    template<typename ProcessorType>
    void addProcessor(std::unique_ptr<ProcessorType> processor);
};
```

### Observer Pattern

Parameter changes are handled through observers:

```cpp
parameterManager->addParameterListener("gain", [this](float value) {
    // Handle parameter change
    updateProcessing(value);
});
```

## Threading Model

### Audio Thread
- **processBlock()**: Real-time audio processing
- **prepareToPlay()**: Audio setup
- **No allocations**: Memory allocation forbidden

### Message Thread
- **GUI updates**: User interface changes
- **Parameter changes**: Non-real-time parameter updates
- **File operations**: Preset loading/saving

### Background Thread
- **Preset scanning**: File system operations
- **Heavy computations**: Non-real-time processing

## Memory Management

### RAII (Resource Acquisition Is Initialization)
- All resources managed through smart pointers
- Automatic cleanup on destruction
- Exception safety guaranteed

### Smart Pointers
```cpp
std::unique_ptr<ParameterManager> parameterManager;
std::unique_ptr<AudioEngine> audioEngine;
std::unique_ptr<PresetManager> presetManager;
```

### Stack Allocation
- Prefer stack allocation for small objects
- Use value semantics where possible
- Minimize heap allocations in audio thread

## Error Handling

### Exception Safety
- **Strong exception safety**: Operations either succeed completely or have no effect
- **No exceptions in audio thread**: Real-time safety
- **RAII**: Automatic resource cleanup

### Error Reporting
```cpp
// Framework provides safe error handling
if (!presetManager->loadPreset(name))
{
    // Handle error gracefully
    showErrorMessage("Failed to load preset: " + name);
}
```

## Plugin Lifecycle

### Initialization
1. Constructor called
2. `initializeParameters()` called
3. Parameter layout created
4. GUI components initialized

### Audio Processing
1. `prepareToPlay()` called with audio specs
2. `processBlock()` called repeatedly
3. Framework handles parameter updates
4. Plugin implements `processAudio()`

### Shutdown
1. `releaseResources()` called
2. Automatic cleanup via RAII
3. Destructors called in reverse order

## Extension Points

### Custom Parameters
```cpp
class MyCustomParameter : public juce::RangedAudioParameter
{
    // Implement custom parameter behavior
};
```

### Custom Effects
```cpp
class MyCustomEffect : public AudioEngine::ProcessorChain
{
    void process(AudioBuffer<float>& buffer) override
    {
        // Custom audio processing
    }
};
```

### Custom GUI Components
```cpp
class MyCustomSlider : public juce::Slider
{
    void paint(Graphics& g) override
    {
        // Custom visual appearance
    }
};
```

## Performance Considerations

### Real-Time Safety
- No memory allocation in audio thread
- Lock-free communication between threads
- Bounded execution time for audio operations

### Optimization
- Template specialization for performance-critical code
- SIMD instructions where appropriate
- Cache-friendly data structures

### Profiling
- Built-in CPU usage monitoring
- Memory usage tracking
- Performance counters available

## Future Extensibility

The architecture is designed for future expansion:

- **Plugin Formats**: Easy addition of new formats (AAX, LV2)
- **DSP Modules**: Expandable effect library
- **GUI Themes**: Multiple visual styles
- **Parameter Types**: Custom parameter implementations
- **Modulation**: Advanced parameter modulation system