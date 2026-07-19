#include "SqlBuilder.h"
#include "TestAssert.h"
#include <vector>

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

void test_insert_single() {
    auto sql = Insert("empno"_f, "ename"_f, "job"_f)
        .values(7788, "maye", "sales")
        .into("emp")
        .to_string();
    ASSERT_TRUE(sql.find("INSERT INTO") != std::string::npos, "Insert basic");
    ASSERT_TRUE(sql.find("empno, ename, job") != std::string::npos, "Insert columns");
    ASSERT_TRUE(sql.find("VALUES") != std::string::npos, "Insert values");
}

void test_insert_multiple() {
    auto sql = Insert("empno"_f, "ename"_f, "job"_f)
        .values(7788, "maye", "sales")
        .values(7789, "rose", "clerk")
        .into("emp")
        .to_string();
    ASSERT_TRUE(sql.find("VALUES") != std::string::npos, "Insert multiple values");
}

void test_insert_chained() {
    auto sql = Insert("empno"_f, "ename"_f, "job"_f)
        .values(7788, "maye", "sales")
        (7789, "rose", "clerk")
        (7790, "jack", "manager")
        .into("emp")
        .to_string();
    ASSERT_TRUE(sql.find("7788") != std::string::npos, "Insert chained first");
    ASSERT_TRUE(sql.find("7789") != std::string::npos, "Insert chained second");
    ASSERT_TRUE(sql.find("7790") != std::string::npos, "Insert chained third");
}

void test_insert_with_table() {
    auto sql = Insert("empno"_f, "ename"_f)
        .values(1, "test")
        .into(Table("emp"))
        .to_string();
    ASSERT_TRUE(sql.find("INSERT INTO emp") != std::string::npos, "Insert with Table object");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing Insert statement\n";
    std::cout << "========================================\n\n";

    test_insert_single();
    test_insert_multiple();
    test_insert_chained();
    test_insert_with_table();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}