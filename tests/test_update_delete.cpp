#include "SqlBuilder.h"
#include "TestAssert.h"
#include <vector>

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

void test_update_basic() {
    auto sql = Update("emp")
        .set("ename"_f = "maye")
        .where("empno"_f == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("UPDATE") != std::string::npos, "Update basic");
    ASSERT_TRUE(sql.find("SET") != std::string::npos, "Update SET");
    ASSERT_TRUE(sql.find("WHERE") != std::string::npos, "Update WHERE");
}

void test_update_multiple_set() {
    auto sql = Update("emp")
        .set("ename"_f = "maye")("sal"_f = 5200)
        .where("empno"_f == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("ename = 'maye'") != std::string::npos, "Update first set");
    ASSERT_TRUE(sql.find("sal = 5200") != std::string::npos, "Update second set");
}

void test_update_with_null() {
    auto sql = Update("emp")
        .set("comm"_f = nullptr)
        .where("empno"_f == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("comm = NULL") != std::string::npos, "Update with NULL");
}

void test_update_field_operation() {
    auto sql = Update("emp")
        .set("sal"_f.assign("sal + 100"))
        .where("empno"_f == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("sal = sal + 100") != std::string::npos, "Update field operation");
}

void test_update_with_table() {
    auto sql = Update(Table("emp"))
        .set("ename"_f = "maye")
        .where("empno"_f == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("UPDATE emp") != std::string::npos, "Update with Table object");
}

void test_delete_basic() {
    auto sql = Delete("emp")
        .where("empno"_f == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("DELETE FROM") != std::string::npos, "Delete basic");
    ASSERT_TRUE(sql.find("WHERE") != std::string::npos, "Delete WHERE");
}

void test_delete_with_table() {
    auto sql = Delete(Table("emp"))
        .where("empno"_f == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("DELETE FROM emp") != std::string::npos, "Delete with Table object");
}

void test_update_subquery() {
    auto sub = Select("empno"_f).from("emp").where("sal"_f > 6000);
    auto sql = Update("emp")
        .set("sal"_f.assign("sal - 5200"))
        .where("empno"_f.in(sub))
        .to_string();
    ASSERT_TRUE(sql.find("IN (") != std::string::npos, "Update with subquery");
}

void test_delete_subquery() {
    auto sub = Select("empno"_f).from("emp").where("sal"_f > 6000);
    auto sql = Delete("emp")
        .where("empno"_f.in(sub))
        .to_string();
    ASSERT_TRUE(sql.find("IN (") != std::string::npos, "Delete with subquery");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing Update/Delete statements\n";
    std::cout << "========================================\n\n";

    test_update_basic();
    test_update_multiple_set();
    test_update_with_null();
    test_update_field_operation();
    test_update_with_table();
    test_delete_basic();
    test_delete_with_table();
    test_update_subquery();
    test_delete_subquery();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}