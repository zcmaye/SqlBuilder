/*****************************************************************//**
 * \file   main.cpp
 * \brief  SQL构建器
 * 
 * \author C语言Plus-zcmaye
 * \date   February 2026
 *********************************************************************/

#include "SqlBuilder.h"
#include "Function.h"

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

using namespace zc::sqlbuilder;

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
	std::println("{}", zc::sqlbuilder::format_value("123"));
	std::println("{}", zc::sqlbuilder::format_value(str));
	std::println("{}", zc::sqlbuilder::format_value(p));
	std::println("{}", zc::sqlbuilder::format_value(sv));

	std::println("{}", zc::sqlbuilder::format_value(name));
	std::println("{}", zc::sqlbuilder::format_value(age));
	std::println("{}", zc::sqlbuilder::format_value(sal));
	std::println("{}", zc::sqlbuilder::format_value(gender));
	std::println("{}", zc::sqlbuilder::format_value(is_manager));

	//empno IN(1,2,3,4,5)
	std::println("{}", zc::sqlbuilder::format_value(std::initializer_list<int>{1, 2, 3, 4, 5}));
	std::println("{}", zc::sqlbuilder::format_value(std::vector{ "hello","maye" }));
	std::println("{}", zc::sqlbuilder::format_value(std::array<double, 5>{3.14, 5.26, 2, 5}));
}

void test_where()
{
	//empno = 1 AND ename = 'maye' OR sal = 6000.02 AND gender = 1 
	Condition condition("empno = 1");
	std::println("{}", condition.to_string());

	auto condi = !(Condition("ename","=",zc::sqlbuilder::format_value("maye")) && 
	Condition("age", "=",zc::sqlbuilder::format_value(123)))||
		Condition("comm", "IS", null);

	std::println("{}", condi.to_string());

	using zc::sqlbuilder::Field;

	auto c = Field("ename") == 1111 && Field("age") > 123 and Field("sal") < 6000.02
		&& Field("sal").between_and(3000, 6000)
		&& Field("comm").is_null()
		&& Field("deptno").is_not_null()
		&& Field("hiredate").between_and("2020-01-01", "2020-12-31")
		&& Field("job") == "sales"
		&& Field("job") > "sales"
		&& Field("job") < "sales"
		&& Field("job") != "sales";

	std::println("{}", c.to_string());

	using namespace zc::sqlbuilder::field_literals;
	c = "ename"_f.in(std::vector<const char*>{"hello", "nihao", "nice"})
		&& "empnno"_f.in(std::vector{ 7788,99,55,6633 })
		&& "ename"_f.not_in(std::vector<const char*>{"hello", "nihao", "nice"})
		&& "empnno"_f.not_in(std::vector{ 7788,99,55,6633 });

	std::println("{}", c.to_string());

	std::println("{}",(std::string)("empno"_f == "maye" && "deptno"_f == 20));
}

void test_update_set()
{
	//UPDATE emp SET ename='maye',job='sales',sal=6000.02,comm=0.1,deptno=20 WHERE empno=1;
	auto set = (zc::sqlbuilder::Assign("ename", zc::sqlbuilder::format_value("maye")), zc::sqlbuilder::Assign("sal", zc::sqlbuilder::format_value(5200)));
	std::println("{}", set.to_string());

	using namespace zc::sqlbuilder::field_literals;
	std::string name = "maye";
	double sal = 5200.05;
	auto sets = ("ename"_f = name, "sal"_f = sal, "comm"_f = 0.1,"deptno"_f = nullptr);
	std::println("{}", sets.to_string());

	std::println("UPDATE emp SET {} ", sets.to_string()); 
}

void test_select()
{
	using namespace zc::sqlbuilder::field_literals;
	std::string sql;

	sql = zc::sqlbuilder::Select("empno"_f, "ename"_f.as("姓名"), "sal"_f, "comm"_f, "emp.deptno"_f)
		.from("emp")
		.join("dept").on("emp.deptno"_f == "dept.deptno"_f)
		.to_string();
	std::println("{}", sql);

	sql = zc::sqlbuilder::Select(zc::sqlbuilder::all)
		.from("emp")
		.left_join("dept")
		.using_("deptno"_f)
		.where("emp.deptno"_f == 20 && "emp.sal"_f > 6000)
		.to_string();
	std::println("{}", sql);


	sql = zc::sqlbuilder::Select(zc::sqlbuilder::fun::count(zc::sqlbuilder::all))
		.from("emp")
		.right_join("dept").on("emp.deptno"_f == "dept.deptno"_f)
		.join("salgrade").as("s").on("emp.sal"_f >= "s.losal"_f && "emp.sal"_f <= "s.hisal"_f)
		.to_string();
	std::println("{}", sql);


	sql = zc::sqlbuilder::Select(zc::sqlbuilder::fun::count("deptno"_f).as("cnt"))
		.from("emp")
		.group_by("deptno"_f, "job"_f)
		.having("cnt"_f > 3)
		.order_by("cnt"_f.desc())
		.limit(10, 20)
		.to_string();
	std::println("{}", sql);
}

void test_optional_select()
{
	using namespace zc::sqlbuilder::field_literals;

	//Emp maye = { 7788,"maye","sales",std::nullopt,"2026-03-12",std::nullopt,100,20 };
	Emp searchEmp;
	searchEmp.ename = "maye";
	searchEmp.sal = "6000.02";

	std::string sql;

	sql = zc::sqlbuilder::Select("empno"_f, "ename"_f.as("姓名"), "sal"_f, "comm"_f, "emp.deptno"_f)
		.from("emp")
		.where("ename"_f == searchEmp.ename && "hiredate"_f > searchEmp.hiredate && "sal"_f == searchEmp.sal)
		.to_string();
	std::println("{}", sql);
}

void test_insert()
{
	using namespace zc::sqlbuilder::field_literals;

	zc::sqlbuilder::Insert("empno"_f, "ename"_f, "job"_f)
		.values(7788,"helo","sales")
		       (7788,"helo","sales")
		       (7788,"helo","sales")
		       (7788,"helo","sales")
		       (7788,"helo","sales")
		.into("emp")
		.to_string();

}

void test_update()
{
	using namespace zc::sqlbuilder::field_literals;

	zc::sqlbuilder::Update("emp")
		.set("ename"_f = "maye")("sal"_f = 5200)
		.where("empno"_f == 7788)
		.to_string();

}

void test_delete() 
{
	using namespace zc::sqlbuilder::field_literals;

	zc::sqlbuilder::Delete("emp")
		.where("empno"_f == 7788)
		.to_string();
}

void test_subquery()
{
	using namespace zc::sqlbuilder::field_literals;
	std::string sql;

	//查询
	std::println("==========SELECT==========");
	//-- 在where中使用子查询
	auto subquery = zc::sqlbuilder::Select("empno"_f).from("emp").where("sal"_f > 5000);
	sql = zc::sqlbuilder::Select("empno"_f, "ename"_f)
		.from("emp")
		.where("empno"_f.in(subquery))
		.to_string();
	std::println("WHERE SUBQUERY:{}", sql);

	//-- 在from中使用子查询
	subquery = zc::sqlbuilder::Select("empno"_f,"ename"_f,"sal"_f,"deptno"_f).from("emp").where("sal"_f > 5000);
	sql = zc::sqlbuilder::Select("empno"_f, "ename"_f)
		.from(subquery).as("e")
		.to_string();
	std::println("FROM SUBQUERY:{}", sql);

	//-- 在select中使用子查询
	subquery = zc::sqlbuilder::Select("sal"_f.as("年薪")).from("emp").where("empno"_f == "e.empno"_f);
	sql = zc::sqlbuilder::Select("empno"_f, zc::sqlbuilder::Field(subquery).as("年薪"), "ename"_f)
		.from("emp").as("e")
		.to_string();
	std::println("SELECT SUBQUERY:{}", sql);

	//更新
	std::println("==========UPDATE==========");
	subquery = zc::sqlbuilder::Select("empno"_f).from("emp").where("sal"_f > 6000);
	sql = zc::sqlbuilder::Update("emp")
		.set("sal"_f = "sal"_f - 5200)
		.where("empno"_f.in(subquery))
		.to_string();
	std::println("UPDATE SUBQUERY:{}", sql);

	//删除
	std::println("==========DELETE==========");
	subquery = zc::sqlbuilder::Select("empno"_f).from("emp").where("sal"_f > 6000);
	sql = zc::sqlbuilder::Delete("emp")
		.where("empno"_f.in(subquery))
		.to_string();
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


	using namespace zc::sqlbuilder::field_literals;

	std::optional<int> empno = 7788;
	std::optional<int> sal = 630;
	std::optional<std::string> ename = "hello' OR 'TRUE' = 'TRUE";
	std::optional<std::string> job;

#ifdef SUPPORT_OPTIONAL
	auto sql = zc::sqlbuilder::Select("empno"_f, "ename"_f)
		.from("emp")
		.where("empno"_f == empno && "ename"_f == ename
			&& "sal"_f > sal && "job"_f == job
		)
		.to_string();

	std::println("{}", sql);

	sql = zc::sqlbuilder::Update("emp")
		.set("sal"_f = "sal"_f - 5200)
		.where("empno"_f == empno && "ename"_f == ename)
		.to_string();

	std::println("{}", sql);
#endif
}

void test_func()
{

	std::println("!!!!!!!!!!!!!!!!!!!!!!!!test_fun!!!!!!!!!!!!!!!!!!!!!!!!!!");
	using namespace zc::sqlbuilder;
	using namespace zc::sqlbuilder::field_literals;

	//max("sal"_f).as("max_sal").to_string();

	//std::println("{}", now().raw_str());

	//std::println("{}",elt(2, "hello", "world", "maye", "zc","empno"_f).raw_str());


	std::println("{} {}", fun::upper("hello").name(), fun::upper("job"_f).as("up_job").name());

	Select("empno"_f, "ename"_f, fun::upper("job"_f));

	"empno"_f == "ename"_f;
	"empno"_f == fun::upper("ename"_f);

	auto c = fun::case_end(1, std::vector{ 1,2,3,4 }, std::vector{ "one","two","three","four" }).as("hello");
	std::println("{}", c.name());
}

int main()
{
	test_total();
	test_func();
	test_optional_select();

	return 0;
}
