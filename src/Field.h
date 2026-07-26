/*****************************************************************//**
 * \file   Field.h
 * \brief  SQL字段
 * 
 * \author Maye
 * \date   July 2026
 *********************************************************************/
#pragma once

#include "Config.h"
#include "Value.h"
#include "Condition.h"
#include "Assign.h"
#include "SqlException.h"

namespace zc::sqlbuilder {

	/** 提前声明查询类 */
	class Select;

	/** 字段类 */
	class Field {
	public:
		Field() = default;
		/**
		 * 通过字段名创建字段.
		 */
		explicit Field(std::string_view  name)
			: _name(name)
		{
		}

		/**
		 * 通过子查询创建字段.
		 */
		explicit Field(const Select& subquery);

		/**
		 * 返回字段名(不包含别名或排序).
.
		 */
		std::string name() const { return _name; }

		/**
		 * 返回字段别名.
		 */
		std::optional<std::string> alias() const { return _alias; }
		bool has_alias() const { return _alias.has_value(); }

		/**
		 * 返回字段字符串(可能包含别名或排序).
		 */
		std::string to_string() const { 
			FIELD_ASSERT(!_name.empty(), "Field name is empty");
			if (_ordering) {
				return std::format("{} {} ", _name, *_ordering);
			}
			if (_alias) {
				return std::format("{} AS '{}' ", _name, *_alias);
			}
			return _name;
		}
		explicit operator std::string() const { return to_string(); }
	public:
		Field& as(const std::string& alias){
			FIELD_ASSERT(!_alias, "Field already has an alias.");
			_alias = alias;
			return *this;
		}
		Field& desc() {
			FIELD_ASSERT(!_ordering, "Field already has an ordering.");
			_ordering = "DESC";
			return *this;
		}
		Field& asc(){
			FIELD_ASSERT(!_ordering, "Field already has an ordering.");
			_ordering = "ASC";
			return *this;
		}

	public:	//字段与值比较
		Condition operator==(const Value& value) const {
			return Condition(_name, "=", value);
		}

		Condition operator>(const Value& value) const {
			return Condition(_name, ">", value);
		}

		Condition operator>=(const Value& value) const {
			return Condition(_name, ">=", value);
		}

		Condition operator<(const Value& value) const {
			return Condition(_name, "<", value);
		}

		Condition operator<=(const Value& value) const {
			return Condition(_name, "<=", value);
		}

		Condition operator!=(const Value& value) const {
			return Condition(_name, "!=", value);
		}

		//comm IS NULL
		Condition is_null() const {
			return Condition(_name, "IS", null);
		}

		//comm IS NOT NULL
		Condition is_not_null() const {
			return Condition(_name, "IS NOT", null);
		}

		//ename LIKE '张%'
		template<typename T, std::enable_if_t<zc::type::traits::is_string_v<T>, int> = 0>
		Condition like(const T& value, std::string_view fmt = "%{}%") const {
			std::string result(fmt.data(), fmt.length());
			if (auto pos = result.find("{}");pos != std::string::npos) {
				result.replace(pos, 2, escape_string(value));
			}
			return Condition(_name, "LIKE", "\'" + result + "\'");
		}

		//hiredate BETWEEN '1990-01-01' AND '2000-01-01'
		Condition between_and(const Value& lower, const Value& upper) const {
			return Condition(_name, "BETWEEN", std::format("{} AND {}", lower, upper));
		}

		//ename IN ('张三', '李四')
		Condition in(const ValueList& value) const {
			return Condition(_name, "IN", value);
		}

		//ename NOT IN ('张三', '李四')
		Condition not_in(const ValueList& value) const {
			return Condition(_name, "NOT IN", value);
		}

	public:	//UPDATE 语句中SET值设置
		AssignmentList operator=(const Value& value) const {
			return Assign(_name, value);
		}

		AssignmentList operator=(std::nullptr_t) const {
			return Assign(_name, null);
		}

		AssignmentList operator=(const Field& field) const {
			return Assign(_name, field);
		}

		// 可选：支持字面量字符串（不加引号）
		AssignmentList assign(const char* raw_sql) const {
			return Assign(_name, Value(raw_sql, Value::Raw));  // 信任用户提供的 SQL
		}
	public://字段与字段比较
		Condition operator==(const Field& field) const {
			return Condition(_name, "=", field.name());
		}

		Condition operator>(const Field& field) const {
			return Condition(_name, ">", field.name());
		}

		Condition operator>=(const Field& field) const {
			return Condition(_name, ">=", field.name());
		}

		Condition operator<(const Field& field) const {
			return Condition(_name, "<", field.name());
		}

		Condition operator<=(const Field& field) const {
			return Condition(_name, "<=", field.name());
		}

		Condition operator!=(const Field& field) const {
			return Condition(_name, "!=", field.name());
		}

		Condition between_and(const Field& lower, const Field& upper) const {
			return Condition(_name, "BETWEEN", std::format("{} AND {}", lower.name(), upper.name()));
		}
	public:	//字段子查询
		Condition operator==(const Select& subquery) const;
		Condition operator!=(const Select& subquery) const;
		Condition operator>(const Select& subquery) const;
		Condition operator<(const Select& subquery) const;
		Condition operator>=(const Select& subquery) const;
		Condition operator<=(const Select& subquery) const;
		Condition in(const Select& subquery) const;
		Condition not_in(const Select& subquery) const;
	public://字段运算
#if 0
		template<typename T, std::enable_if_t<(zc::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !zc::type::traits::is_container_v<T>, int> = 0>
		Field operator+(T value) const {
			return Field(std::format("{} + {}", _name, format_value(value)));
		}

		template<typename T, std::enable_if_t<(zc::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !zc::type::traits::is_container_v<T>, int> = 0>
		Field operator-(T value) const {
			return Field(std::format("{} - {}", _name, format_value(value)));
		}

		template<typename T, std::enable_if_t<(zc::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !zc::type::traits::is_container_v<T>, int> = 0>
		Field operator*(T value) const {
			return Field(std::format("{} * {}", _name, format_value(value)));
		}

		template<typename T, std::enable_if_t<(zc::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !zc::type::traits::is_container_v<T>, int> = 0>
		Field operator/(T value) const {
			return Field(std::format("{} / {}", _name, format_value(value)));
		}

		template<typename T, std::enable_if_t<(zc::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !zc::type::traits::is_container_v<T>, int> = 0>
		Field operator%(T value) const {
			return Field(std::format("{} % {}", _name, format_value(value)));
		}
#else
		Field operator+(const Value& value) const {
			return Field(std::format("{} + {}", _name, value));
		}

		Field operator-(const Value& value) const {
			return Field(std::format("{} - {}", _name, value));
		}

		Field operator*(const Value& value) const {
			return Field(std::format("{} * {}", _name, value));
		}

		Field operator/(const Value& value) const {
			return Field(std::format("{} / {}", _name, value));
		}

		Field operator%(const Value& value) const {
			return Field(std::format("{} % {}", _name, value));
		}
#endif

#include "detail/optional_impl.inc"
#include "detail/oatpp_impl.inc"
	private:
		std::string _name;						/*!字段名*/
		std::optional<std::string> _alias;		/*!别名*/
		std::optional<std::string> _ordering;	/*!排序*/
	};

	namespace field_literals {
		inline Field operator""_c(const char* name, std::size_t size) {
			return Field(std::string(name, size));
		}
		inline Field operator""_f(const char* name, std::size_t size) {
			return Field(std::string(name, size));
		}
		inline Field operator""_field(const char* name, std::size_t size) {
			return Field(std::string(name, size));
		}
	}
	inline std::string format_value(const Field& val) {
		return std::format("{}", static_cast<std::string>(val));
	}

	inline static Field all{ "*" };

	//使用逗号连接所有列
	inline std::string join(const std::vector<Field>& fields, const std::string& delimiter = ",")
	{
		std::string result;
		bool first = true;
		for (auto& c : fields) {
			if (!first) {
				result += delimiter;
			}
			result += c.to_string();
			first = false;
		}
		return result;
	}

	template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && (std::conjunction_v<std::is_same<std::decay_t<Args>, Field>...>), int> = 0>
	inline std::string join(Args&&... args) {
		//解包并格式化值
		return join({ std::forward<Args>(args)... });
	}
}
