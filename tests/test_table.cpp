#include "Table.h"
#include "TestAssert.h"

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::table_literals;

void test_table_basic() {
    Table t("emp");
    ASSERT_EQUAL("emp", t.name(), "Table basic name");
    ASSERT_EQUAL("emp", t.to_string(), "Table basic to_string");
    ASSERT_FALSE(t.has_alias(), "Table has no alias");
}

void test_table_default_constructor() {
    Table t;
    ASSERT_EQUAL("", t.name(), "Table default constructor name");
    ASSERT_FALSE(t, "Table default is false");
}

void test_table_alias() {
    Table t = Table("emp").as("e");
    ASSERT_TRUE(t.has_alias(), "Table has alias");
    ASSERT_EQUAL("e", t.alias().value(), "Table alias value");
    ASSERT_EQUAL("emp AS e ", t.to_string(), "Table with alias to_string");
}

void test_table_operator_bool() {
    Table t1("emp");
    ASSERT_TRUE(t1, "Table with name is true");

    Table t2;
    ASSERT_FALSE(t2, "Table without name is false");
}

void test_table_from_string_view() {
    std::string_view sv = "dept";
    Table t(sv);
    ASSERT_EQUAL("dept", t.name(), "Table from string_view");
}

void test_table_from_char_ptr() {
    const char* ptr = "salgrade";
    Table t(ptr);
    ASSERT_EQUAL("salgrade", t.name(), "Table from char pointer");
}

void test_table_literal() {
    Table t = "emp"_t;
    ASSERT_EQUAL("emp", t.name(), "Table literal _t");

    Table t2 = "dept"_table;
    ASSERT_EQUAL("dept", t2.name(), "Table literal _table");
}

void test_table_alias_not_overwrite() {
    Table t = Table("emp").as("e");
    ASSERT_THROWS(t.as("f"), "Table alias overwrite throws");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing Table class\n";
    std::cout << "========================================\n\n";

    test_table_basic();
    test_table_default_constructor();
    test_table_alias();
    test_table_operator_bool();
    test_table_from_string_view();
    test_table_from_char_ptr();
    test_table_literal();
    test_table_alias_not_overwrite();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}