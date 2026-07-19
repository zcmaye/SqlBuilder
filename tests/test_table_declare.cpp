#include "TableDeclare.h"
#include "SqlBuilder.h"
#include "TestAssert.h"

using namespace zc::sqlbuilder;

DECLARE_TABLE(Dept, dept, deptno, dname, loc);
DECLARE_TABLE(Emp, emp, empno, ename, job, hiredate, sal, mgr, comm, deptno);
DECLARE_TABLE(Salgrade, salgrade, grade, losal, hisal);

void test_table_declare_basic() {
    const table::Emp emp;
    ASSERT_EQUAL("emp", emp.table_name, "Table declare table_name");
}

void test_table_declare_operator_table() {
    const table::Emp emp;
    Table t = emp;
    ASSERT_EQUAL("emp", t.name(), "Table declare operator Table");
}

void test_table_declare_field_name() {
    const table::Emp emp;
    ASSERT_EQUAL("emp.empno", emp.empno.name(), "Table declare field empno");
    ASSERT_EQUAL("emp.ename", emp.ename.name(), "Table declare field ename");
    ASSERT_EQUAL("emp.sal", emp.sal.name(), "Table declare field sal");
    ASSERT_EQUAL("emp.deptno", emp.deptno.name(), "Table declare field deptno");
}

void test_table_declare_multiple_tables() {
    const table::Emp emp;
    const table::Dept dept;
    ASSERT_EQUAL("emp.empno", emp.empno.name(), "Table declare Emp table");
    ASSERT_EQUAL("dept.deptno", dept.deptno.name(), "Table declare Dept table");
    ASSERT_EQUAL("dept.dname", dept.dname.name(), "Table declare Dept dname");
    ASSERT_EQUAL("dept.loc", dept.loc.name(), "Table declare Dept loc");
}

void test_table_declare_select() {
    const table::Emp emp;
    auto sql = Select(emp.empno, emp.ename)
        .from(emp)
        .to_string();
    ASSERT_TRUE(sql.find("emp.empno") != std::string::npos, "Table declare select field");
    ASSERT_TRUE(sql.find("FROM emp") != std::string::npos, "Table declare select from");
}

void test_table_declare_select_all() {
    auto sql = Select(all).from(table::Emp{})
        .to_string();
    ASSERT_EQUAL("SELECT * FROM emp", sql, "Table declare select all");
}

void test_table_declare_join() {
    const table::Emp emp;
    const table::Dept dept;
    auto sql = Select(all)
        .from(emp)
        .join(dept)
        .on(emp.deptno == dept.deptno)
        .to_string();
    ASSERT_TRUE(sql.find("JOIN dept") != std::string::npos, "Table declare join");
    ASSERT_TRUE(sql.find("ON emp.deptno = dept.deptno") != std::string::npos, "Table declare join on");
}

void test_table_declare_where() {
    const table::Emp emp;
    auto sql = Select(all)
        .from(emp)
        .where(emp.sal > 1000)
        .to_string();
    ASSERT_TRUE(sql.find("WHERE emp.sal > 1000") != std::string::npos, "Table declare where");
}

void test_table_declare_order_by() {
    const table::Emp emp;
    auto sql = Select(all)
        .from(emp)
        .order_by(Field(emp.sal).desc())
        .to_string();
    ASSERT_TRUE(sql.find("ORDER BY") != std::string::npos, "Table declare order by");
}

void test_table_declare_limit() {
    const table::Emp emp;
    auto sql = Select(all)
        .from(emp)
        .limit(10)
        .to_string();
    ASSERT_TRUE(sql.find("LIMIT 10") != std::string::npos, "Table declare limit");
}

void test_table_declare_salgrade() {
    const table::Salgrade s;
    ASSERT_EQUAL("salgrade.grade", s.grade.name(), "Table declare Salgrade grade");
    ASSERT_EQUAL("salgrade.losal", s.losal.name(), "Table declare Salgrade losal");
    ASSERT_EQUAL("salgrade.hisal", s.hisal.name(), "Table declare Salgrade hisal");
}

void test_table_declare_between_and() {
    const table::Emp emp;
    const table::Salgrade s;
    auto sql = Select(all)
        .from(emp)
        .join(s)
        .on(emp.sal.between_and(s.losal, s.hisal))
        .to_string();
    ASSERT_TRUE(sql.find("BETWEEN") != std::string::npos, "Table declare between_and");
}

void test_table_declare_update() {
    const table::Emp emp;
    auto sql = Update(emp)
        .set(emp.ename = "maye")
        .where(emp.empno == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("UPDATE emp") != std::string::npos, "Table declare update");
    ASSERT_TRUE(sql.find("SET emp.ename = 'maye'") != std::string::npos, "Table declare update set");
}

void test_table_declare_delete() {
    const table::Emp emp;
    auto sql = Delete(emp)
        .where(emp.empno == 7788)
        .to_string();
    ASSERT_TRUE(sql.find("DELETE FROM emp") != std::string::npos, "Table declare delete");
}

void test_table_declare_insert() {
    const table::Emp emp;
    auto sql = Insert(emp.empno, emp.ename)
        .values(7788, "maye")
        .into(emp)
        .to_string();
    ASSERT_TRUE(sql.find("INSERT INTO emp") != std::string::npos, "Table declare insert");
    ASSERT_TRUE(sql.find("emp.empno, emp.ename") != std::string::npos, "Table declare insert columns");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "Testing Table Declare (DECLARE_TABLE)\n";
    std::cout << "========================================\n\n";

    test_table_declare_basic();
    test_table_declare_operator_table();
    test_table_declare_field_name();
    test_table_declare_multiple_tables();
    test_table_declare_select();
    test_table_declare_select_all();
    test_table_declare_join();
    test_table_declare_where();
    test_table_declare_order_by();
    test_table_declare_limit();
    test_table_declare_salgrade();
    test_table_declare_between_and();
    test_table_declare_update();
    test_table_declare_delete();
    test_table_declare_insert();

    test::summary();
    return test::g_fail_count > 0 ? 1 : 0;
}