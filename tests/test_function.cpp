#include "Function.h"
#include "SqlBuilder.h"
#include "TestAssert.h"
#include <vector>

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;
using namespace zc::sqlbuilder::fun;

void test_function_aggregate_count() {
    Field f = count("empno"_f);
    ASSERT_EQUAL("COUNT(empno)", f.name(), "Function count");

    Field f2 = count_all();
    ASSERT_EQUAL("COUNT(*)", f2.name(), "Function count_all");
}

void test_function_aggregate_max_min() {
    Field f = max("sal"_f);
    ASSERT_EQUAL("MAX(sal)", f.name(), "Function max");

    Field f2 = min("sal"_f);
    ASSERT_EQUAL("MIN(sal)", f2.name(), "Function min");
}

void test_function_aggregate_avg_sum() {
    Field f = avg("sal"_f);
    ASSERT_EQUAL("AVG(sal)", f.name(), "Function avg");

    Field f2 = sum("sal"_f);
    ASSERT_EQUAL("SUM(sal)", f2.name(), "Function sum");
}

void test_function_string_upper_lower() {
    Field f = upper("ename"_f);
    ASSERT_EQUAL("UPPER(ename)", f.name(), "Function upper with field");

    Field f2 = lower("ename");
    ASSERT_EQUAL("LOWER('ename')", f2.name(), "Function lower with string");
}

void test_function_string_substr() {
    Field f = substr("ename", 1, 5);
    ASSERT_EQUAL("SUBSTR('ename',1,5)", f.name(), "Function substr");

    Field f2 = substr("ename", 1);
    ASSERT_EQUAL("SUBSTR('ename',1)", f2.name(), "Function substr default len");
}

void test_function_string_trim() {
    Field f = trim("ename");
    ASSERT_EQUAL("TRIM('ename')", f.name(), "Function trim");

    Field f2 = ltrim("ename");
    ASSERT_EQUAL("LTRIM('ename')", f2.name(), "Function ltrim");

    Field f3 = rtrim("ename");
    ASSERT_EQUAL("RTRIM('ename')", f3.name(), "Function rtrim");
}

void test_function_string_length() {
    Field f = length("ename");
    ASSERT_EQUAL("LENGTH('ename')", f.name(), "Function length");
}

void test_function_date_now() {
    Field f = now();
    ASSERT_EQUAL("NOW()", f.name(), "Function now");
}

void test_function_date_curdate() {
    Field f = curdate();
    ASSERT_EQUAL("CURDATE()", f.name(), "Function curdate");
}

void test_function_date_curtime() {
    Field f = curtime();
    ASSERT_EQUAL("CURTIME()", f.name(), "Function curtime");
}

void test_function_date_year() {
    Field f = year("hiredate");
    ASSERT_EQUAL("YEAR('hiredate')", f.name(), "Function year");
}

void test_function_case_end() {
    Field f = case_end("deptno"_f, 
        std::vector{10, 20, 30}, 
        std::vector<std::string>{"ACCOUNTING", "RESEARCH", "SALES"});
    ASSERT_TRUE(f.name().find("CASE") != std::string::npos, "Function case_end has CASE");
    ASSERT_TRUE(f.name().find("END)") != std::string::npos, "Function case_end has END");
}

void test_function_case_end_with_alias() {
    Field f = case_end("deptno"_f, 
        std::vector{10, 20}, 
        std::vector<std::string>{"A", "B"}).as("dept_name");
    ASSERT_EQUAL("dept_name", f.alias().value(), "Function case_end with alias");
}

void test_function_in_select() {
    auto sql = Select(count_all())
        .from("emp")
        .to_string();
    ASSERT_EQUAL("SELECT COUNT(*) FROM emp", sql, "Function in Select");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing SQL Functions\n";
    std::cout << "========================================\n\n";

    test_function_aggregate_count();
    test_function_aggregate_max_min();
    test_function_aggregate_avg_sum();
    test_function_string_upper_lower();
    test_function_string_substr();
    test_function_string_trim();
    test_function_string_length();
    test_function_date_now();
    test_function_date_curdate();
    test_function_date_curtime();
    test_function_date_year();
    test_function_case_end();
    test_function_case_end_with_alias();
    test_function_in_select();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}