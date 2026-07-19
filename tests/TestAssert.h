#pragma once

#include <iostream>
#include <string>
#include <format>

namespace test {

inline int g_test_count = 0;
inline int g_pass_count = 0;
inline int g_fail_count = 0;

inline void report(bool passed, const std::string& test_name, const std::string& message = "") {
    g_test_count++;
    if (passed) {
        g_pass_count++;
        std::cout << "[PASS] " << test_name << std::endl;
    } else {
        g_fail_count++;
        std::cout << "[FAIL] " << test_name;
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << std::endl;
    }
}

inline void summary() {
    std::cout << "\n========================================" << std::endl;
    std::cout << std::format("Total: {}, Passed: {}, Failed: {}", 
        g_test_count, g_pass_count, g_fail_count) << std::endl;
    std::cout << "========================================" << std::endl;
}

#define ASSERT_TRUE(expr, name) \
    test::report((expr) == true, name, std::format("Expected true, got {}", (expr) ? "true" : "false"))

#define ASSERT_FALSE(expr, name) \
    test::report((expr) == false, name, std::format("Expected false, got {}", (expr) ? "true" : "false"))

#define ASSERT_EQUAL(expected, actual, name) \
    test::report((expected) == (actual), name, \
        std::format("Expected '{}', got '{}'", (expected), (actual)))

#define ASSERT_NOT_EQUAL(expected, actual, name) \
    test::report((expected) != (actual), name, \
        std::format("Expected not '{}', but got '{}'", (expected), (actual)))

#define ASSERT_THROWS(expr, name) \
    do { \
        bool threw = false; \
        try { expr; } \
        catch (...) { threw = true; } \
        test::report(threw, name, "Expected exception, but none was thrown"); \
    } while (false)

#define ASSERT_NO_THROW(expr, name) \
    do { \
        bool threw = false; \
        try { expr; } \
        catch (...) { threw = true; } \
        test::report(!threw, name, "Unexpected exception was thrown"); \
    } while (false)

} // namespace test