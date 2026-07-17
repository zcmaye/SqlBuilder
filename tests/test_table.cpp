#include "Table.h"
#include "Field.h"
#include "SqlBuilder.h"
#include "BaseZcMacro.hpp"
#include <iostream>

using namespace hdy::tool::sql;
#if 1
//#define FIELD_NAME(NAME) Field NAME{#NAME};
//#define FIELD_NAME(NAME) Field NAME{[this]{return std::format("{}.{}",name(), #NAME);}()};
//#define TABLE_NAME(NAME)\
//	constexpr const char* name() const { return #NAME; }\
//	operator Table() const { return Table(name()); }

//#define ZC_MACRO_SINGLE_TO(NAME) Field NAME{[this]{return std::format("{}.{}",name(), #NAME);}()};
//#define ZC_MACRO_PROCESS_SINGLE(pair) ZC_MACRO_SINGLE_TO ( pair )


#define ZC_MACRO_SINGLE_TO(NAME) Field NAME{[this]{return std::format("{}.{}",table_name, #NAME);}()};
#define ZC_MACRO_PROCESS_SINGLE(pair) ZC_MACRO_SINGLE_TO (pair)

/**
 * 定义数据库表格，方便在语句中使用.
 * 定义出的表格结构在命名空间table下：
 *	namespace table{
 *		struct CLASS{
 *			Field field1;
 *			Field field2;
 *			Field field3;
 *		}; 
 * }
 * @param CLASS 类名
 * @param NAME 表名
 * @param ... 字段名
 */
#define DECLAR_TABLE(CLASS, NAME,...) \
	namespace table{\
	struct CLASS { \
		inline static const char* table_name{#NAME};\
		operator Table() const { return Table(table_name); }\
		ZC_MACRO_FOR_EACH(ZC_MACRO_PROCESS_SINGLE,__VA_ARGS__) \
	};}

DECLAR_TABLE(Dept, dept, deptno, dname, loc);
DECLAR_TABLE(Emp, emp, empno, ename,job, hiredate, sal, mgr, comm, deptno);
DECLAR_TABLE(Salgrade, salgrade, grade, losal, hisal);

#else
#define ZC_MACRO_PAIR_TO(TYPE,NAME)\
	Field NAME{[this]{return std::format("{}.{}",name(), #NAME);}()};\
	TYPE _##NAME;

#define ZC_MACRO_PROCESS_PAIR(pair) ZC_MACRO_PAIR_TO pair

#define DECLAR_TABLE_PAIR(CLASS, TBNAME,...) \
	namespace table{\
	struct CLASS { \
		TABLE_NAME(TBNAME) \
		ZC_MACRO_FOR_EACH(ZC_MACRO_PROCESS_PAIR,__VA_ARGS__) \
	};}

//DECLAR_TABLE_PAIR(Dept, dept, int, deptno, std::string, dname, std::string, loc);
//DECLAR_TABLE_PAIR(Emp, emp, empno, ename,job, hiredate, sal, mgr, comm, deptno);

DECLAR_TABLE_PAIR(Dept, dept, 
	(int, deptno), 
	(std::string, dname), 
	(std::string, loc)
);

template<typename T>
class FieldValue {
public:
	using value_type = T;
public:
	FieldValue(const std::string& fieldName) : _field(fieldName), _value() {}

	FieldValue(const std::string& fieldName, T&& value) : _field(fieldName), _value(std::move(value)) {}

	operator Field() const { return _field; }
	FieldValue& operator=(T&& value) { _value = std::move(value); return *this; }
private:
	Field _field;
	value_type _value;
};

struct Salgrade {
	FieldValue<int> grade{ [this] {return std::format("{}.{}",name(),"grade");}(),2 };
	FieldValue<float> losal{ [this] {return std::format("{}.{}",name(),"losal");}()};
	FieldValue<float> hisal{ [this] {return std::format("{}.{}",name(),"hisal");}()};

	constexpr const char* name() const { return "salgrade"; }
	operator Table() const { return Table(name()); }
};

#endif

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
