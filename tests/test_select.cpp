#include "SqlBuilder.h"
#include "Function.h"
#include "TestAssert.h"
#include <vector>

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;
using namespace zc::sqlbuilder::fun;

void test_select_basic() {
    auto sql = Select("empno"_f, "ename"_f)
        .from("emp")
        .to_string();
    ASSERT_EQUAL("SELECT empno, ename FROM emp", sql, "Select basic");
}

void test_select_all() {
    auto sql = Select(all).from("emp").to_string();
    ASSERT_EQUAL("SELECT * FROM emp", sql, "Select all");
}

void test_select_with_where() {
    auto sql = Select("empno"_f, "ename"_f)
        .from("emp")
        .where("empno"_f == 7788)
        .to_string();
    ASSERT_EQUAL("SELECT empno, ename FROM emp WHERE empno = 7788", sql, "Select with where");
}

void test_select_with_alias() {
    auto sql = Select("empno"_f, "ename"_f.as("姓名"))
        .from("emp")
        .to_string();
    ASSERT_TRUE(sql.find("姓名") != std::string::npos, "Select with alias");
}

void test_select_group_by() {
    auto sql = Select("deptno"_f, count("empno"_f).as("cnt"))
        .from("emp")
        .group_by("deptno"_f)
        .to_string();
    ASSERT_TRUE(sql.find("GROUP BY") != std::string::npos, "Select with group_by");
}

void test_select_having() {
    auto sql = Select("deptno"_f, count("empno"_f).as("cnt"))
        .from("emp")
        .group_by("deptno"_f)
        .having("cnt"_f > 3)
        .to_string();
    ASSERT_TRUE(sql.find("HAVING") != std::string::npos, "Select with having");
}

void test_select_order_by() {
    auto sql = Select("empno"_f, "ename"_f)
        .from("emp")
        .order_by("sal"_f.desc())
        .to_string();
    ASSERT_TRUE(sql.find("ORDER BY") != std::string::npos, "Select with order_by");
    ASSERT_TRUE(sql.find("DESC") != std::string::npos, "Select with DESC");
}

void test_select_limit() {
    auto sql = Select("empno"_f, "ename"_f)
        .from("emp")
        .limit(10)
        .to_string();
    ASSERT_EQUAL("SELECT empno, ename FROM emp LIMIT 10", sql, "Select with limit");

    sql = Select("empno"_f, "ename"_f)
        .from("emp")
        .limit(20, 10)
        .to_string();
    ASSERT_EQUAL("SELECT empno, ename FROM emp LIMIT 10 OFFSET 20", sql, "Select with limit offset");
}

void test_select_inner_join() {
    auto sql = Select(all)
        .from("emp")
        .join("dept")
        .on("emp.deptno"_f == "dept.deptno"_f)
        .to_string();
    ASSERT_TRUE(sql.find("JOIN") != std::string::npos, "Select with inner join");
    ASSERT_TRUE(sql.find("ON") != std::string::npos, "Select with ON");
}

void test_select_left_join() {
    auto sql = Select(all)
        .from("emp")
        .left_join("dept")
        .to_string();
    ASSERT_TRUE(sql.find("LEFT JOIN") != std::string::npos, "Select with left join");
}

void test_select_right_join() {
    auto sql = Select(all)
        .from("emp")
        .right_join("dept")
        .to_string();
    ASSERT_TRUE(sql.find("RIGHT JOIN") != std::string::npos, "Select with right join");
}

void test_select_full_join() {
    auto sql = Select(all)
        .from("emp")
        .full_join("dept")
        .to_string();
    ASSERT_TRUE(sql.find("FULL JOIN") != std::string::npos, "Select with full join");
}

void test_select_using() {
    auto sql = Select(all)
        .from("emp")
        .left_join("dept")
        .using_("deptno"_f)
        .to_string();
    ASSERT_TRUE(sql.find("USING") != std::string::npos, "Select with using");
}

void test_select_table_alias() {
    auto sql = Select(all)
        .from("emp")
        .join("salgrade").as("s")
        .on("emp.sal"_f >= "s.losal"_f)
        .to_string();
    ASSERT_TRUE(sql.find("AS 's'") != std::string::npos, "Select with table alias");
}

void test_select_subquery_where() {
    auto sub = Select("empno"_f).from("emp").where("sal"_f > 5000);
    auto sql = Select("empno"_f, "ename"_f)
        .from("emp")
        .where("empno"_f.in(sub))
        .to_string();
    ASSERT_TRUE(sql.find("IN (") != std::string::npos, "Select with subquery IN");
}

void test_select_subquery_from() {
    auto sub = Select("empno"_f, "ename"_f).from("emp").where("sal"_f > 5000);
    auto sql = Select("empno"_f, "ename"_f)
        .from(sub).as("e")
        .to_string();
    ASSERT_TRUE(sql.find("FROM (") != std::string::npos, "Select with subquery FROM");
}

void test_select_with_function() {
    auto sql = Select(count_all())
        .from("emp")
        .to_string();
    ASSERT_EQUAL("SELECT COUNT(*) FROM emp", sql, "Select with count_all");

    sql = Select(max("sal"_f).as("max_sal"))
        .from("emp")
        .to_string();
    ASSERT_TRUE(sql.find("MAX") != std::string::npos, "Select with max");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing Select statement\n";
    std::cout << "========================================\n\n";

    test_select_basic();
    test_select_all();
    test_select_with_where();
    test_select_with_alias();
    test_select_group_by();
    test_select_having();
    test_select_order_by();
    test_select_limit();
    test_select_inner_join();
    test_select_left_join();
    test_select_right_join();
    test_select_full_join();
    test_select_using();
    test_select_table_alias();
    test_select_subquery_where();
    test_select_subquery_from();
    test_select_with_function();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}