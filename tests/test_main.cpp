#include <juce_core/juce_core.h>
#include <iostream>
#include <vector>
#include <functional>

// Simple test framework
class TestRunner
{
public:
    static TestRunner& getInstance()
    {
        static TestRunner instance;
        return instance;
    }

    void addTest(const juce::String& name, std::function<bool()> testFunc)
    {
        tests.push_back({name, testFunc});
    }

    int runAllTests()
    {
        int passed = 0;
        int failed = 0;

        std::cout << "Running " << tests.size() << " tests..." << std::endl;

        for (const auto& test : tests)
        {
            std::cout << "  Running: " << test.name << "... ";
            
            try
            {
                if (test.func())
                {
                    std::cout << "PASSED" << std::endl;
                    ++passed;
                }
                else
                {
                    std::cout << "FAILED" << std::endl;
                    ++failed;
                }
            }
            catch (const std::exception& e)
            {
                std::cout << "EXCEPTION: " << e.what() << std::endl;
                ++failed;
            }
        }

        std::cout << "\n========================================" << std::endl;
        std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
        std::cout << "========================================\n" << std::endl;

        return (failed == 0) ? 0 : 1;
    }

private:
    struct Test
    {
        juce::String name;
        std::function<bool()> func;
    };

    std::vector<Test> tests;
};

#define TEST(name) \
    static bool test_##name(); \
    static struct TestRegistrar_##name { \
        TestRegistrar_##name() { \
            TestRunner::getInstance().addTest(#name, test_##name); \
        } \
    } testRegistrar_##name; \
    static bool test_##name()

// Basic tests for framework components
TEST(Framework_Placeholder)
{
    // This is a placeholder test.
    // Replace with actual tests for your framework components.
    int a = 1;
    int b = 1;
    return a == b;
}

// Test ParameterManager
#include "../LIVE-SYSTEMS/include/ParameterManager.h"
#include <juce_audio_processors/juce_audio_processors.h>

class MockProcessor : public juce::AudioProcessor
{
public:
    MockProcessor() : AudioProcessor(BusesProperties()) {}
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    const juce::String getName() const override { return "Mock"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
};

TEST(ParameterManager_Basic)
{
    MockProcessor processor;
    ParameterManager paramMgr(processor);
    
    ParameterManager::ParameterInfo info;
    info.id = "test_param";
    info.name = "Test Param";
    info.defaultValue = 0.5f;
    info.minValue = 0.0f;
    info.maxValue = 1.0f;
    
    paramMgr.addFloatParameter(info);
    
    // Check default value
    if (std::abs(paramMgr.getParameterValue("test_param") - 0.5f) > 0.001f)
        return false;
        
    // Set new value
    paramMgr.setParameterValue("test_param", 0.8f);
    
    // Check new value
    if (std::abs(paramMgr.getParameterValue("test_param") - 0.8f) > 0.001f)
        return false;
        
    return true;
}



int main(int argc, char* argv[])
{
    juce::ignoreUnused(argc, argv);
    
    std::cout << "LIVE-SYSTEMS Framework Tests" << std::endl;
    std::cout << "============================\n" << std::endl;

    return TestRunner::getInstance().runAllTests();
}
