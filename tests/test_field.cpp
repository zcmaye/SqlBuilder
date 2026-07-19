#include "Field.h"
#include "TestAssert.h"
#include <vector>

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

void test_field_basic() {
    Field f("ename");
    ASSERT_EQUAL("ename", f.name(), "Field name");
    ASSERT_FALSE(f.has_alias(), "Field has no alias");
    ASSERT_EQUAL("ename", f.to_string(), "Field to_string");
}

void test_field_alias() {
    Field f = Field("ename").as("姓名");
    ASSERT_TRUE(f.has_alias(), "Field has alias");
    ASSERT_EQUAL("姓名", f.alias().value(), "Field alias value");
}

void test_field_ordering() {
    Field f1 = Field("sal").asc();
    ASSERT_EQUAL("sal ASC ", f1.to_string(), "Field ascending");

    Field f2 = Field("sal").desc();
    ASSERT_EQUAL("sal DESC ", f2.to_string(), "Field descending");
}

void test_field_comparison() {
    ASSERT_EQUAL("ename = 'maye'", ("ename"_f == "maye").to_string(), "Field == string");
    ASSERT_EQUAL("empno = 7788", ("empno"_f == 7788).to_string(), "Field == int");
    ASSERT_EQUAL("sal > 5000", ("sal"_f > 5000).to_string(), "Field > int");
    ASSERT_EQUAL("sal >= 5000", ("sal"_f >= 5000).to_string(), "Field >= int");
    ASSERT_EQUAL("sal < 5000", ("sal"_f < 5000).to_string(), "Field < int");
    ASSERT_EQUAL("sal <= 5000", ("sal"_f <= 5000).to_string(), "Field <= int");
    ASSERT_EQUAL("ename != 'maye'", ("ename"_f != "maye").to_string(), "Field != string");
}

void test_field_null_check() {
    ASSERT_EQUAL("comm IS NULL", ("comm"_f.is_null()).to_string(), "Field is_null");
    ASSERT_EQUAL("comm IS NOT NULL", ("comm"_f.is_not_null()).to_string(), "Field is_not_null");
}

void test_field_like() {
    ASSERT_EQUAL("ename LIKE '%maye%'", ("ename"_f.like("maye")).to_string(), "Field like default");
    ASSERT_EQUAL("ename LIKE 'maye%'", ("ename"_f.like("maye", "{}%")).to_string(), "Field like prefix");
    ASSERT_EQUAL("ename LIKE '%maye'", ("ename"_f.like("maye", "%{}")).to_string(), "Field like suffix");
}

void test_field_between() {
    ASSERT_EQUAL("sal BETWEEN 3000 AND 8000", ("sal"_f.between_and(3000, 8000)).to_string(), "Field between_and int");
    ASSERT_EQUAL("hiredate BETWEEN '2020-01-01' AND '2020-12-31'", 
        ("hiredate"_f.between_and("2020-01-01", "2020-12-31")).to_string(), "Field between_and string");
}

void test_field_in() {
    ASSERT_EQUAL("empno IN (7788, 7789, 7790)", 
        ("empno"_f.in(std::vector{7788, 7789, 7790})).to_string(), "Field in vector int");
    ASSERT_EQUAL("ename IN ('hello', 'world')", 
        ("ename"_f.in(std::vector<std::string>{"hello", "world"})).to_string(), "Field in vector string");
}

void test_field_not_in() {
    ASSERT_EQUAL("empno NOT IN (7788, 7789, 7790)", 
        ("empno"_f.not_in(std::vector{7788, 7789, 7790})).to_string(), "Field not_in vector int");
}

void test_field_field_comparison() {
    ASSERT_EQUAL("sal = mgr", ("sal"_f == "mgr"_f).to_string(), "Field == Field");
    ASSERT_EQUAL("sal > comm", ("sal"_f > "comm"_f).to_string(), "Field > Field");
}

void test_field_arithmetic() {
    ASSERT_EQUAL("sal + 100", ("sal"_f + 100).name(), "Field + int");
    ASSERT_EQUAL("sal - 100", ("sal"_f - 100).name(), "Field - int");
    ASSERT_EQUAL("sal * 1.5", ("sal"_f * 1.5).name(), "Field * double");
    ASSERT_EQUAL("sal / 2", ("sal"_f / 2).name(), "Field / int");
    ASSERT_EQUAL("sal % 7", ("sal"_f % 7).name(), "Field % int");
}

void test_field_to_string() {
    ASSERT_EQUAL("ename", static_cast<std::string>(Field("ename")), "Field cast to string");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing Field class\n";
    std::cout << "========================================\n\n";

    test_field_basic();
    test_field_alias();
    test_field_ordering();
    test_field_comparison();
    test_field_null_check();
    test_field_like();
    test_field_between();
    test_field_in();
    test_field_not_in();
    test_field_field_comparison();
    test_field_arithmetic();
    test_field_to_string();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}