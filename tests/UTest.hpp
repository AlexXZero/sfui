#ifndef UTEST_HPP_INCLUDED
#define UTEST_HPP_INCLUDED

#include <iostream>
#include <vector>

#define UTEST_PRINT(fmt, ...) fprintf(stderr, "%s:%d: %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define UTEST_FAIL(message, ...) do { UTEST_PRINT("FAILED! " message, ##__VA_ARGS__); throw UTest::TestFailException(); } while (0)

#define UTEST_ASSERT_TRUE(actual) \
    if (!(actual)) UTEST_FAIL("%s is not TRUE", #actual)

#define UTEST_ASSERT_FALSE(actual) \
    if (actual) UTEST_FAIL("%s is not FALSE", #actual)

#define UTEST_ASSERT_EQUAL(expected, actual) \
    if ((actual) != (expected)) UTEST_FAIL("%s != %s", #actual, #expected)

#define UTEST_ASSERT_EQUAL_UINT32(expected, actual) \
    if ((uint32_t)(actual) != (uint32_t)(expected)) UTEST_FAIL("FAILED! %u != %u", actual, expected)

#define UTEST_ASSERT_EQUAL_VECTOR(expected, actual) \
    if ((actual) != (expected)) UTEST_FAIL("%s(%u) != %s(%u)", #actual, actual.size(), #expected, expected.size())

#define UTEST_DBG(msg, ...) //UTEST_PRINT(msg, ##__VA_ARGS__)

#define UTEST(Name) \
    static void Test##Name(); \
    static UTest::TestRegistry::TestRegistration Test##Name##Registration(Test##Name, "Test"#Name); \
    static void Test##Name()

#define UTEST_SETUP(Name) \
    static void Setup##Name(); \
    static UTest::TestRegistry::SetupRegistration Setup##Name##Registration(Setup##Name); \
    static void Setup##Name()

#define UTEST_TEARDOWN(Name) \
    static void Teardown##Name(); \
    static UTest::TestRegistry::TeardownRegistration Teardown##Name##Registration(Teardown##Name); \
    static void Teardown##Name()


namespace UTest {

struct TestFailException {};
// TODO: struct TestSkipException {};

class TestRegistry {
public:
    using TestFunction = void(*)();
    using SetupFunction = void(*)();
    using TeardownFunction = void(*)();

    struct TestRegistration {
        TestRegistration(TestFunction test, std::string name) { TestRegistry::RegisterTest(test, name); }
    };
    struct SetupRegistration {
        SetupRegistration(SetupFunction setup) { TestRegistry::RegisterSetup(setup); }
    };
    struct TeardownRegistration {
        TeardownRegistration(TeardownFunction teardown) { TestRegistry::RegisterTearDown(teardown); }
    };

    static int main(int argc, const char* argv[]) {
        std::cout << "================================" << std::endl;
        std::cout << "Start testing " << argv[0] << ":" << std::endl;

        const std::size_t totalTests = Instance().m_tests.size();
        std::size_t passedTests = 0;
        std::size_t failedTests = 0;

        for (auto& setup : Instance().m_setups) {
            (*setup)();
        }
        for (auto& test : Instance().m_tests) {
            try {
                (*test.function)();
                ++passedTests;
            } catch (TestFailException) {
                ++failedTests;
            } catch (const std::exception& e) {
                fprintf(stderr, "%s: Exception thrown: %s\n", test.name.c_str(), e.what());
                ++failedTests;
            } catch (...) {
                fprintf(stderr, "%s: Unknown exception thrown\n", test.name.c_str());
                ++failedTests;
            }
        }
        for (auto& teardown : Instance().m_teardowns) {
            (*teardown)();
        }

        // Print statistics
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string RESET = "\033[0m";
        std::cout << "--------------------------------" << std::endl;
        std::cout << "  Total Tests: " << totalTests << std::endl;
        if (passedTests == totalTests) std::cout << GREEN;
        std::cout << "  Passed: " << passedTests << std::endl;
        if (passedTests == totalTests) std::cout << RESET;
        if (failedTests > 0) std::cout << RED;
        std::cout << "  Failed: " << failedTests << std::endl;
        if (failedTests > 0) std::cout << RESET;
        return failedTests > 0 ? -1 : 0;
    }

private:
    TestRegistry() = default; // Private constructor for singleton pattern
    static TestRegistry& Instance() { static TestRegistry instance; return instance; }
    static void RegisterTest(TestFunction test, std::string name) { Instance().m_tests.push_back({test, name}); }
    static void RegisterSetup(SetupFunction setup) { Instance().m_setups.push_back(setup); }
    static void RegisterTearDown(TeardownFunction teardown) { Instance().m_teardowns.push_back(teardown); }

private:
    struct Test {
        TestFunction function;
        std::string name;
    };

private:
    std::vector<Test> m_tests;
    std::vector<SetupFunction> m_setups;
    std::vector<TeardownFunction> m_teardowns;
};

}

int main(int argc, const char* argv[])
{
    return UTest::TestRegistry::main(argc, argv);
}

#endif // UTEST_HPP_INCLUDED
