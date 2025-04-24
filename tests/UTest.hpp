#ifndef UTEST_HPP_INCLUDED
#define UTEST_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <tuple>
#include <locale>
#include <codecvt>
#include <string_view>
#include <cinttypes>

#define UTEST_PRINT(fmt, ...) fprintf(stderr, "%s:%d: %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define UTEST_FAIL(message, ...) do { UTEST_PRINT("Assertion failed: " message, ##__VA_ARGS__); throw UTest::TestFailException(); } while (0)

#define UTEST_ASSERT_TRUE(actual) \
    if (!(actual)) UTEST_FAIL("%s is not TRUE", #actual)

#define UTEST_ASSERT_FALSE(actual) \
    if (actual) UTEST_FAIL("%s is not FALSE", #actual)

#define UTEST_ASSERT_EQUAL(expected, actual)                                \
    do {                                                                    \
        auto expected_ = (expected);                                        \
        auto actual_ = (actual);                                            \
        if (actual_ != expected_)                                           \
            UTEST_FAIL("%s != %s, Expected: \"%s\", Actual: \"%s\"",        \
                       #expected, #actual,                                  \
                       UTest::ToString(expected_).c_str(),                  \
                       UTest::ToString(actual_).c_str());                   \
    } while (0)

#define UTEST_ASSERT_NOT_EQUAL(expected, actual)                            \
    do {                                                                    \
        auto expected_ = (expected);                                        \
        auto actual_ = (actual);                                            \
        if (actual_ == expected_)                                           \
            UTEST_FAIL("%s == %s, Expected: \"%s\", Actual: \"%s\"",        \
                       #expected, #actual,                                  \
                       UTest::ToString(expected_).c_str(),                  \
                       UTest::ToString(actual_).c_str());                   \
    } while (0)

// Macros for vectors (e.g., std::vector)
#define UTEST_ASSERT_EQUAL_VECTOR(expected, actual)                         \
    do {                                                                    \
        const auto& expected_ = (expected);                                 \
        const auto& actual_ = (actual);                                     \
        if (expected_.size() != actual_.size())                             \
            UTEST_FAIL("%s != %s, Vector sizes differ: Expected: %zu, Actual: %zu", \
                       #expected, #actual,                                  \
                       expected_.size(), actual_.size());                   \
        if (actual_ != expected_)                                           \
            UTEST_FAIL("%s != %s, Vector elements differ: Expected: %s, Actual: %s", \
                       #expected, #actual,                                  \
                       UTest::ToString(expected_).c_str(),                  \
                       UTest::ToString(actual_).c_str());                   \
    } while (0)

#define UTEST_ASSERT_EQUAL_INT8(expected, actual) \
    UTEST_ASSERT_EQUAL(static_cast<int8_t>(expected), static_cast<int8_t>(actual))

#define UTEST_ASSERT_EQUAL_INT16(expected, actual) \
    UTEST_ASSERT_EQUAL(static_cast<int16_t>(expected), static_cast<int16_t>(actual))

#define UTEST_ASSERT_EQUAL_INT32(expected, actual) \
    UTEST_ASSERT_EQUAL(static_cast<int32_t>(expected), static_cast<int32_t>(actual))

#define UTEST_ASSERT_EQUAL_INT64(expected, actual) \
    UTEST_ASSERT_EQUAL(static_cast<int64_t>(expected), static_cast<int64_t>(actual))

#define UTEST_ASSERT_EQUAL_UINT8(expected, actual) \
    UTEST_ASSERT_EQUAL(static_cast<uint8_t>(expected), static_cast<uint8_t>(actual))

#define UTEST_ASSERT_EQUAL_UINT16(expected, actual) \
    UTEST_ASSERT_EQUAL(static_cast<uint16_t>(expected), static_cast<uint16_t>(actual))

#define UTEST_ASSERT_EQUAL_UINT32(expected, actual) \
    UTEST_ASSERT_EQUAL(static_cast<uint32_t>(expected), static_cast<uint32_t>(actual))

#define UTEST_ASSERT_EQUAL_UINT64(expected, actual) \
    UTEST_ASSERT_EQUAL(static_cast<uint64_t>(expected), static_cast<uint64_t>(actual))

#define UTEST_ASSERT_EQUAL_DOUBLE(expected, actual, epsilon)                \
    do {                                                                    \
        double expected_ = (expected);                                      \
        double actual_ = (actual);                                          \
        if (std::abs(actual_ - expected_) > (epsilon))                      \
            UTEST_FAIL("%s != %s, Expected: %.6f, Actual: %.6f",            \
                       #expected, #actual, expected_, actual_);             \
    } while (0)

#define UTEST_ASSERT_EQUAL_HEX(expected, actual)                            \
    do {                                                                    \
        uint64_t expected_ = (expected);                                    \
        uint64_t actual_ = (actual);                                        \
        if (actual_ != expected_)                                           \
            UTEST_FAIL("%s != %s, Expected: 0x%" PRIx64 ", Actual: 0x%" PRIx64, \
                       #expected, #actual, expected_, actual_);             \
    } while (0)

#define UTEST_ASSERT_EQUAL_FLOAT(expected, actual, epsilon) \
    UTEST_ASSERT_EQUAL_DOUBLE(expected, actual, epsilon)

#define UTEST_ASSERT_EQUAL_STRING(expected, actual)                        \
    UTEST_ASSERT_EQUAL(std::string_view(expected), std::string_view(actual))

#define UTEST_ASSERT_GRATER(expected, actual) \
    if ((actual) <= (expected)) UTEST_FAIL("%s >= %s", #actual, #expected)

#define UTEST_ASSERT_LESS(expected, actual) \
    if ((actual) >= (expected)) UTEST_FAIL("%s <= %s", #actual, #expected)

#define UTEST_ASSERT_INT_WITHIN(delta, expected, actual) \
    if (std::abs((int)(actual) - (int)(expected)) > (delta)) UTEST_FAIL("%d != %d +/- %d", (int)(actual), (int)(expected), (int)(delta))

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

// Helper for static_assert to produce compile-time errors for unsupported types
template <typename>
struct always_false : std::false_type {};

// Helper for checking if a type is iterable
template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                  decltype(std::end(std::declval<T>()))>> : std::true_type {};

// Helper for checking if a type is a pair
template <typename T>
struct is_pair : std::false_type {};

template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

// Helper for tuple-like types
template <typename T>
struct is_tuple : std::false_type {};

template <typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type {};

// Helper class for converting tested values to std::string
template <typename T>
std::string ToString(const T& value)
{
    if constexpr (std::is_same<T, std::string>::value) {
        return value;
    } else if constexpr (std::is_same<T, const char*>::value || std::is_same<T, char*>::value) {
        return std::string(value);
    } else if constexpr (std::is_same_v<T, const wchar_t*> || std::is_same_v<T, wchar_t*>) {
        // Using std::wstring_convert to convert from wide string to UTF-8
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(value);
    } else if constexpr (std::is_enum<T>::value) {
        return std::to_string(static_cast<typename std::underlying_type<T>::type>(value));
    } else if constexpr (std::is_arithmetic<T>::value) {
        return std::to_string(value);
    } else if constexpr (std::is_convertible<T, std::string_view>::value) {
        return std::string(value);
    } else if constexpr (is_iterable<T>::value) {
        // Supports STL containers (e.g., vector, list, set)
        std::string result = "[";
        for (auto it = std::begin(value); it != std::end(value); ++it) {
            if (it != std::begin(value)) result += ", ";
            result += ToString(*it);
        }
        result += "]";
        return result;
    } else if constexpr (is_pair<T>::value) {
        return "(" + ToString(value.first) + ", " + ToString(value.second) + ")";
    } else if constexpr (is_tuple<T>::value) {
        return std::apply(
            [](const auto&... args) {
                std::string result = "(";
                bool first = true;
                ((result += (first ? "" : ", ") + ToString(args), first = false), ...);
                result += ")";
                return result;
            },
            value);
    } else if constexpr (std::is_convertible<T, long long>::value) {
        // Handle types that have a conversion operator to long long
        return std::to_string(static_cast<long long>(value));
    } else {
        static_assert(always_false<T>::value, "UTest::ToString: Unsupported type");
    }
}

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
