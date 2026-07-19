#include "Condition.h"
#include "Field.h"
#include "TestAssert.h"

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

void test_condition_basic() {
    Condition c("empno = 1");
    ASSERT_EQUAL("empno = 1", c.to_string(), "Condition basic");
    ASSERT_FALSE(c.empty(), "Condition not empty");
}

void test_condition_empty() {
    Condition c;
    ASSERT_TRUE(c.empty(), "Condition default empty");
    ASSERT_EQUAL("", c.to_string(), "Condition empty to_string");
}

void test_condition_and() {
    Condition c1("empno = 1");
    Condition c2("ename = 'maye'");
    Condition c3 = c1 && c2;
    ASSERT_EQUAL("(empno = 1 AND ename = 'maye')", c3.to_string(), "Condition &&");
}

void test_condition_or() {
    Condition c1("empno = 1");
    Condition c2("ename = 'maye'");
    Condition c3 = c1 || c2;
    ASSERT_EQUAL("(empno = 1 OR ename = 'maye')", c3.to_string(), "Condition ||");
}

void test_condition_not() {
    Condition c("empno = 1");
    Condition nc = !c;
    ASSERT_EQUAL("NOT empno = 1", nc.to_string(), "Condition !");
}

void test_condition_and_empty() {
    Condition c1;
    Condition c2("ename = 'maye'");
    Condition c3 = c1 && c2;
    ASSERT_EQUAL("ename = 'maye'", c3.to_string(), "Condition && with empty left");

    Condition c4 = c2 && c1;
    ASSERT_EQUAL("ename = 'maye'", c4.to_string(), "Condition && with empty right");
}

void test_condition_or_empty() {
    Condition c1;
    Condition c2("ename = 'maye'");
    Condition c3 = c1 || c2;
    ASSERT_EQUAL("ename = 'maye'", c3.to_string(), "Condition || with empty left");

    Condition c4 = c2 || c1;
    ASSERT_EQUAL("ename = 'maye'", c4.to_string(), "Condition || with empty right");
}

void test_condition_chained() {
    Condition c = ("empno"_f == 1) && ("ename"_f == "maye") || ("sal"_f > 5000);
    ASSERT_TRUE(c.to_string().find("AND") != std::string::npos, "Chained condition has AND");
    ASSERT_TRUE(c.to_string().find("OR") != std::string::npos, "Chained condition has OR");
}

void test_condition_nested() {
    Condition c = !("empno"_f == 1 && "ename"_f == "maye");
    ASSERT_EQUAL("NOT (empno = 1 AND ename = 'maye')", c.to_string(), "Nested NOT with AND");
}

void test_condition_to_string() {
    Condition c("empno = 1");
    ASSERT_EQUAL("empno = 1", static_cast<const std::string&>(c), "Condition cast to string");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing Condition class\n";
    std::cout << "========================================\n\n";

    test_condition_basic();
    test_condition_empty();
    test_condition_and();
    test_condition_or();
    test_condition_not();
    test_condition_and_empty();
    test_condition_or_empty();
    test_condition_chained();
    test_condition_nested();
    test_condition_to_string();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}