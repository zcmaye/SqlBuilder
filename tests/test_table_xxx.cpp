#include "Table.h"
#include "Field.h"
#include "SqlBuilder.h"
#include "TableDeclare.h"
#include <iostream>

using namespace zc::sqlbuilder;

DECLARE_TABLE(Dept, dept, deptno, dname, loc);
DECLARE_TABLE(Emp, emp, empno, ename,job, hiredate, sal, mgr, comm, deptno);
DECLARE_TABLE(Salgrade, salgrade, grade, losal, hisal);

void test_table_2()
{
	const table::Emp emp;
	const table::Dept dept;
	const table::Salgrade s;

	std::cout << (emp.sal = 100, emp.ename = "maye").to_string() << std::endl;

	std::cout << "select: " << Select(all).from(table::Emp{}).to_string() << std::endl;
	std::cout << "select: " << Select(emp.empno, emp.ename, emp.sal).from(table::Emp{}).to_string() << std::endl;

	auto sql = Select(all).from(emp).join(dept)
		.on(emp.deptno == dept.deptno)
		.where(emp.sal > 1000)
		.order_by(Field(emp.sal).asc()).limit(10).to_string();
	std::cout << "select: " << sql << std::endl;

	sql = Select(all).from(emp).join(s).on(emp.sal.between_and(s.losal, s.hisal)).to_string();
	std::cout << "select: " << sql << std::endl;

	sql = Select(emp.ename, emp.sal, s.grade)
		.from(emp).join(s).on(emp.sal.between_and(s.losal, s.hisal)).to_string();
	std::cout << "select: " << sql << std::endl;
}

void test_table_1()
{
	std::cout << Table("emp").to_string() << std::endl;
	std::cout << Table("emp").as("e").to_string() << std::endl;
}

int main()
{
	try {
		test_table_2();
	}
	catch (const SqlException& e) {
		std::cout << "[" << e.module_name() << " Error]:" << e.what() << std::endl;
	}
	return 0;
}
