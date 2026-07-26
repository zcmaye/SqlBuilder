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
void test_insert_value_if()
{
    struct User {
        uint32_t user_id;
        std::string username;
        std::string nickname;
        std::string password;
        bool is_active;
    };

	using namespace zc::sqlbuilder;
	using namespace zc::sqlbuilder::field_literals;

	User user{ 1001, "admin", "管理员", "123456", true };

	auto sql = Insert("user_id"_c, "username"_c, 
        "nickname"_c, "password"_c, "is_active"_c)
		.values_object(user, [](const User& user) {
            return ValueList{
                user.user_id,
                user.username, 
                user.nickname,
                user.password,
                user.is_active 
            };
			})
		.into("sys_user")
		.to_string();

    ASSERT_TRUE(sql.find("(1001, 'admin', '管理员', '123456', true)") != std::string::npos, "Insert values_for ok");
}

void test_insert_value_for()
{
	using namespace zc::sqlbuilder;
	using namespace zc::sqlbuilder::field_literals;

	std::vector<int> userIds{ 1001,1002,1004,1005,1006 };
	int roleId = 2002;

    auto sql = Insert("role_id"_c, "user_id"_c)
        .values_for(userIds, [roleId](int userId) {
        return ValueList{ Value(roleId),Value(userId) };
        })
		.into("sys_user_role")
		.to_string();

    ASSERT_TRUE(sql.find("(2002, 1001), (2002, 1002)") != std::string::npos, "Insert values_for ok");
}

int main() {
	std::cout << "========================================\n";
	std::cout << "Testing Insert statement\n";
	std::cout << "========================================\n\n";

	test_insert_single();
	test_insert_multiple();
	test_insert_chained();
	test_insert_with_table();
	test_insert_value_if();
	test_insert_value_for();

	test::summary();
	return test::g_fail_count > 0 ? 1 : 0;
}