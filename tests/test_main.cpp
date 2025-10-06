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


int main(int argc, char* argv[])
{
    juce::ignoreUnused(argc, argv);
    
    std::cout << "LIVE-SYSTEMS Framework Tests" << std::endl;
    std::cout << "============================\n" << std::endl;

    return TestRunner::getInstance().runAllTests();
}
