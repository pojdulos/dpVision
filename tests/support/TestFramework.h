#pragma once

#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct TestCase {
    const char* name;
    void (*fn)();
};

inline std::vector<TestCase>& allTests()
{
    static std::vector<TestCase> tests;
    return tests;
}

struct TestRegistrar {
    TestRegistrar(const char* name, void (*fn)())
    {
        allTests().push_back({name, fn});
    }
};

#define TEST_CASE(name) \
    static void name(); \
    static TestRegistrar name##_registrar(#name, &name); \
    static void name()

#define REQUIRE(expr) \
    do { \
        if (!(expr)) { \
            std::ostringstream oss; \
            oss << "Requirement failed: " #expr " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while (0)

#define REQUIRE_EQ(lhs, rhs) \
    do { \
        const auto& lhsValue = (lhs); \
        const auto& rhsValue = (rhs); \
        if (!(lhsValue == rhsValue)) { \
            std::ostringstream oss; \
            oss << "Requirement failed: " #lhs " == " #rhs " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while (0)
