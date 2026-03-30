/*****************************************************************//**
 * \file   main.cpp
 * \brief  SQL构建器
 * 
 * \author 长沙宏定义课堂-顽石老师
 * \date   February 2026
 *********************************************************************/

#include "SqlBuilder.h"
#include "Funcation.h"

#include <print>
#include <string>
#include <format>
#include <sstream>

#include <array>

/*
* 写SQL语句：
* 1，SQL比较复杂，难写，字符串拼接容易出错
* 2，SQL格式不统一，容易出错
* 
* Where条件生成
*/

using namespace hdy::tool::sql;

struct Emp {
	int empno;
	std::optional<std::string> ename;
	std::optional<std::string> job;
	std::optional<int> mgr;
	std::optional<std::string> hiredate;
	std::optional<std::string> sal;
	std::optional<int> comm;
	std::optional<int> deptno;
};

void test_value()
{
	std::string name = "maye";
	std::string age = "123";
	double sal = 6000.02;
	int gender = 1;
	bool is_manager = true;

	char str[] = "123";
	char* p = str;
	std::string_view sv = "123";
	std::println("{}", hdy::tool::sql::format_value("123"));
	std::println("{}", hdy::tool::sql::format_value(str));
	std::println("{}", hdy::tool::sql::format_value(p));
	std::println("{}", hdy::tool::sql::format_value(sv));

	std::println("{}", hdy::tool::sql::format_value(name));
	std::println("{}", hdy::tool::sql::format_value(age));
	std::println("{}", hdy::tool::sql::format_value(sal));
	std::println("{}", hdy::tool::sql::format_value(gender));
	std::println("{}", hdy::tool::sql::format_value(is_manager));

	//empno IN(1,2,3,4,5)
	std::println("{}", hdy::tool::sql::format_value(std::initializer_list<int>{1, 2, 3, 4, 5}));
	std::println("{}", hdy::tool::sql::format_value(std::vector{ "hello","maye" }));
	std::println("{}", hdy::tool::sql::format_value(std::array<double, 5>{3.14, 5.26, 2, 5}));
}

void test_where()
{
	//empno = 1 AND ename = 'maye' OR sal = 6000.02 AND gender = 1 
	Condition condition("empno = 1");
	std::println("{}", condition.sql());

	auto condi = !(Condition("ename","=",hdy::tool::sql::format_value("maye")) && 
	Condition("age", "=",hdy::tool::sql::format_value(123)))||
		Condition("comm", "IS", "NULL");

	std::println("{}", condi.sql());

	using hdy::tool::sql::Column;

	auto c = Column("ename") == 1111 && Column("age") > 123 and Column("sal") < 6000.02
		&& Column("sal").between_and(3000, 6000)
		&& Column("comm").is_null()
		&& Column("deptno").is_not_null()
		&& Column("hiredate").between_and("2020-01-01", "2020-12-31")
		&& Column("job") == "sales"
		&& Column("job") > "sales"
		&& Column("job") < "sales"
		&& Column("job") != "sales";

	std::println("{}", c.sql());

	using namespace hdy::tool::sql::literals;
	c = "ename"_c.in(std::vector<const char*>{"hello", "nihao", "nice"})
		&& "empnno"_c.in(std::vector{ 7788,99,55,6633 })
		&& "ename"_c.not_in(std::vector<const char*>{"hello", "nihao", "nice"})
		&& "empnno"_c.not_in(std::vector{ 7788,99,55,6633 });

	std::println("{}", c.sql());

	std::println("{}",(std::string)("empno"_c == "maye" && "deptno"_c == 20));
}

void test_update_set()
{
	//UPDATE emp SET ename='maye',job='sales',sal=6000.02,comm=0.1,deptno=20 WHERE empno=1;
	auto set = (hdy::tool::sql::Set("ename", hdy::tool::sql::format_value("maye")), hdy::tool::sql::Set("sal", hdy::tool::sql::format_value(5200)));
	std::println("{}", set.sql());

	using namespace hdy::tool::sql::literals;
	std::string name = "maye";
	double sal = 5200.05;
	set = ("ename"_c = name, "sal"_c = sal, "comm"_c = 0.1,"deptno"_c = nullptr);
	std::println("{}", set.sql());

	std::println("UPDATE emp SET {} ", set.sql()); 
}

void test_select()
{
	using namespace hdy::tool::sql::literals;
	std::string sql;

	sql = hdy::tool::sql::Select("empno"_c, "ename"_c.as("姓名"), "sal"_c, "comm"_c, "emp.deptno"_c)
		.from("emp")
		.join("dept").on("emp.deptno"_c == "dept.deptno"_c)
		.sql();
	std::println("{}", sql);

	sql = hdy::tool::sql::Select(hdy::tool::sql::all)
		.from("emp")
		.left_join("dept")
		.using_("deptno"_c)
		.where("emp.deptno"_c == 20 && "emp.sal"_c > 6000)
		.sql();
	std::println("{}", sql);


	sql = hdy::tool::sql::Select(hdy::tool::sql::count(hdy::tool::sql::all))
		.from("emp")
		.right_join("dept").on("emp.deptno"_c == "dept.deptno"_c)
		.join("salgrade").as("s").on("emp.sal"_c >= "s.losal"_c && "emp.sal"_c <= "s.hisal"_c)
		.sql();
	std::println("{}", sql);


	sql = hdy::tool::sql::Select(hdy::tool::sql::count("deptno"_c).as("cnt"))
		.from("emp")
		.group_by("deptno"_c, "job"_c)
		.having("cnt"_c > 3)
		.order_by("cnt"_c.desc())
		.limit(10, 20)
		.sql();
	std::println("{}", sql);
}

void test_optional_select()
{
	using namespace hdy::tool::sql::literals;

	//Emp maye = { 7788,"maye","sales",std::nullopt,"2026-03-12",std::nullopt,100,20 };
	Emp searchEmp;
	searchEmp.ename = "maye";
	searchEmp.sal = "6000.02";

	std::string sql;

	sql = hdy::tool::sql::Select("empno"_c, "ename"_c.as("姓名"), "sal"_c, "comm"_c, "emp.deptno"_c)
		.from("emp")
		.where("ename"_c == searchEmp.ename && "hiredate"_c > searchEmp.hiredate && "sal"_c == searchEmp.sal)
		.sql();
	std::println("{}", sql);
}

void test_insert()
{
	using namespace hdy::tool::sql::literals;

	hdy::tool::sql::Insert("empno"_c, "ename"_c, "job"_c)
		.values(7788,"helo","sales")
		       (7788,"helo","sales")
		       (7788,"helo","sales")
		       (7788,"helo","sales")
		       (7788,"helo","sales")
		.into("emp")
		.sql();

}

void test_update()
{
	using namespace hdy::tool::sql::literals;

	hdy::tool::sql::Update("emp")
		.set("ename"_c = "maye")("sal"_c = 5200)
		.where("empno"_c == 7788)
		.sql();

}

void test_delete() 
{
	using namespace hdy::tool::sql::literals;

	hdy::tool::sql::Delete("emp")
		.where("empno"_c == 7788)
		.sql();
}

void test_subquery()
{
	using namespace hdy::tool::sql::literals;
	std::string sql;

	//查询
	std::println("==========SELECT==========");
	//-- 在where中使用子查询
	auto subquery = hdy::tool::sql::Select("empno"_c).from("emp").where("sal"_c > 5000);
	sql = hdy::tool::sql::Select("empno"_c, "ename"_c)
		.from("emp")
		.where("empno"_c.in(subquery))
		.sql();
	std::println("WHERE SUBQUERY:{}", sql);

	//-- 在from中使用子查询
	subquery = hdy::tool::sql::Select("empno"_c,"ename"_c,"sal"_c,"deptno"_c).from("emp").where("sal"_c > 5000);
	sql = hdy::tool::sql::Select("empno"_c, "ename"_c)
		.from(subquery).as("e")
		.sql();
	std::println("FROM SUBQUERY:{}", sql);

	//-- 在select中使用子查询
	subquery = hdy::tool::sql::Select("sal"_c.as("年薪")).from("emp").where("empno"_c == "e.empno"_c);
	sql = hdy::tool::sql::Select("empno"_c, hdy::tool::sql::Column(subquery).as("年薪"), "ename"_c)
		.from("emp").as("e")
		.sql();
	std::println("SELECT SUBQUERY:{}", sql);

	//更新
	std::println("==========UPDATE==========");
	subquery = hdy::tool::sql::Select("empno"_c).from("emp").where("sal"_c > 6000);
	sql = hdy::tool::sql::Update("emp")
		.set("sal"_c = "sal"_c - 5200)
		.where("empno"_c.in(subquery))
		.sql();
	std::println("UPDATE SUBQUERY:{}", sql);

	//删除
	std::println("==========DELETE==========");
	subquery = hdy::tool::sql::Select("empno"_c).from("emp").where("sal"_c > 6000);
	sql = hdy::tool::sql::Delete("emp")
		.where("empno"_c.in(subquery))
		.sql();
	std::println("DELETE SUBQUERY:{}", sql);
}

void test_total()
{
	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_value!!!!!!!!!!!!!!!!!!!!!!!!!!");
	test_value();
	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_where!!!!!!!!!!!!!!!!!!!!!!!!!!");
	test_where();
	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_update_set!!!!!!!!!!!!!!!!!!!!!!!!!!");
	test_update_set();
	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_select!!!!!!!!!!!!!!!!!!!!!!!!!!");
	test_select();
	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_insert!!!!!!!!!!!!!!!!!!!!!!!!!!");
	test_insert();
	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_update!!!!!!!!!!!!!!!!!!!!!!!!!!");
	test_update();
	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_delete!!!!!!!!!!!!!!!!!!!!!!!!!!");
	test_delete();
	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_subquery!!!!!!!!!!!!!!!!!!!!!!!!!!");
	test_subquery();


	using namespace hdy::tool::sql::literals;

	std::optional<int> empno = 7788;
	std::optional<int> sal = 630;
	std::optional<std::string> ename = "hello' OR 'TRUE' = 'TRUE";
	std::optional<std::string> job;

#ifdef SUPPORT_OPTIONAL
	auto sql = hdy::tool::sql::Select("empno"_c, "ename"_c)
		.from("emp")
		.where("empno"_c == empno && "ename"_c == ename
			&& "sal"_c > sal && "job"_c == job
		)
		.sql();

	std::println("{}", sql);

	sql = hdy::tool::sql::Update("emp")
		.set("sal"_c = "sal"_c - 5200)
		.where("empno"_c == empno && "ename"_c == ename)
		.sql();

	std::println("{}", sql);
#endif
}

void test_func()
{
	using namespace hdy::tool::sql;
	using namespace hdy::tool::sql::literals;

	//max("sal"_c).as("max_sal").sql();

	//std::println("{}", now().raw_str());

	//std::println("{}",elt(2, "hello", "world", "maye", "hdy","empno"_c).raw_str());


	std::println("{} {}", upper("hello").name(), upper("job"_c).as("up_job").name());

	Select("empno"_c, "ename"_c, upper("job"_c));

	"empno"_c == "ename"_c;
	"empno"_c == upper("ename"_c);

	auto c = case_end(1, std::vector{ 1,2,3,4 }, std::vector{ "one","two","three","four" }).as("hello");
	std::println("{}", c.name());
}

int main()
{
	test_total();
	test_func();
	test_optional_select();

	return 0;
}
