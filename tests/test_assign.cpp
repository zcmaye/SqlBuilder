#include "Assign.h"
#include "Field.h"
#include "TestAssert.h"

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

void test_assign_basic() {
    Assign a("ename", "'maye'");
    ASSERT_EQUAL("ename = 'maye'", a.to_string(), "Assign basic");
    ASSERT_FALSE(a.empty(), "Assign not empty");
}

void test_assign_empty() {
    Assign a;
    ASSERT_TRUE(a.empty(), "Assign default empty");
}

void test_assign_to_string() {
    Assign a("ename", "'maye'");
    ASSERT_EQUAL("ename = 'maye'", static_cast<std::string>(a), "Assign cast to string");
}

void test_assignment_list_single() {
    AssignmentList al("ename", "'maye'");
    ASSERT_EQUAL("ename = 'maye'", al.to_string(), "AssignmentList single");
}

void test_assignment_list_from_assign() {
    AssignmentList al(Assign("ename", "'maye'"));
    ASSERT_EQUAL("ename = 'maye'", al.to_string(), "AssignmentList from Assign");
}

void test_assignment_list_add() {
    AssignmentList al;
    al.add(Assign("ename", "'maye'"));
    al.add(Assign("sal", "5200"));
    ASSERT_EQUAL("ename = 'maye', sal = 5200", al.to_string(), "AssignmentList add");
}

void test_assignment_list_add_list() {
    AssignmentList al1("ename", "'maye'");
    AssignmentList al2("sal", "5200");
    al1.add(al2);
    ASSERT_EQUAL("ename = 'maye', sal = 5200", al1.to_string(), "AssignmentList add list");
}

void test_assignment_list_operator_plus_equals() {
    AssignmentList al1("ename", "'maye'");
    AssignmentList al2("sal", "5200");
    al1 += al2;
    ASSERT_EQUAL("ename = 'maye', sal = 5200", al1.to_string(), "AssignmentList operator+=");
}

void test_assignment_list_operator_comma() {
    AssignmentList al = (Assign("ename", "'maye'"), Assign("sal", "5200"));
    ASSERT_EQUAL("ename = 'maye', sal = 5200", al.to_string(), "AssignmentList comma operator");
}

void test_assignment_list_empty() {
    AssignmentList al;
    ASSERT_TRUE(al.empty(), "AssignmentList default empty");
    ASSERT_EQUAL("", al.to_string(), "AssignmentList empty to_string");
}

void test_field_operator_equal() {
    AssignmentList al = "ename"_f = "maye";
    ASSERT_EQUAL("ename = 'maye'", al.to_string(), "Field = string");

    al = "sal"_f = 5200;
    ASSERT_EQUAL("sal = 5200", al.to_string(), "Field = int");

    al = "comm"_f = nullptr;
    ASSERT_EQUAL("comm = NULL", al.to_string(), "Field = nullptr");
}

void test_field_operator_equal_field() {
    AssignmentList al = "sal"_f = "mgr"_f;
    ASSERT_EQUAL("sal = mgr", al.to_string(), "Field = Field");
}

void test_field_raw_set() {
    AssignmentList al = "sal"_f.raw_set("sal + 100");
    ASSERT_EQUAL("sal = sal + 100", al.to_string(), "Field raw_set");
}

void test_chained_assignments() {
    AssignmentList al = ("ename"_f = "maye", "sal"_f = 5200, "comm"_f = nullptr);
    ASSERT_EQUAL("ename = 'maye', sal = 5200, comm = NULL", al.to_string(), "Chained assignments");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing Assign/AssignmentList\n";
    std::cout << "========================================\n\n";

    test_assign_basic();
    test_assign_empty();
    test_assign_to_string();
    test_assignment_list_single();
    test_assignment_list_from_assign();
    test_assignment_list_add();
    test_assignment_list_add_list();
    test_assignment_list_operator_plus_equals();
    test_assignment_list_operator_comma();
    test_assignment_list_empty();
    test_field_operator_equal();
    test_field_operator_equal_field();
    test_field_raw_set();
    test_chained_assignments();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}