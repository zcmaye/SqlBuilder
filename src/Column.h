#pragma once

#include "Config.h"
#include "FormatValue.h"
#include "Condition.h"
#include "Set.h"

namespace hdy::tool::sql {

	class Select;

	/** 列(字段) */
	class Column {
	public:
		/**
		 * 通过字段名创建字段.
		 */
		explicit Column(std::string_view  name)
			: _column_name(name)
		{
		}

		/**
		 * 通过子查询创建字段.
		 */
		explicit Column(const Select& subquery);

		/**
		 * 返回字段名.
		 */
		std::string column_name() const { return _column_name; }

		/**
		 * 返回字段别名.
		 */
		std::string alias_name() const { return _alias_name; }

		/**
		 * 返回包含字段名+别名的整个字段.
		 */
		explicit operator std::string()const { 
			std::string full_name = _column_name;
			if (!_alias_name.empty()) {
				full_name += std::format(" AS '{}' ", _alias_name);
			}
			else if (!_ordering.empty()) {
				full_name += std::format(" {} ", _ordering);
			}
			return full_name;
		}

		std::string name() const { return static_cast<std::string>(*this); }

		Column& as(const std::string& alias){
			_alias_name = alias;
			return *this;
		}
		Column& desc() {
			_ordering = "DESC";
			return *this;
		}
		Column& asc(){
			_ordering = "ASC";
			return *this;
		}

	public:	//字段与值比较
		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator==(T value) const {
			return Condition(_column_name, "=", format_value<T>(value));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator>(T value) const {
			return Condition(_column_name, ">", format_value<T>(value));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator>=(T value) const {
			return Condition(_column_name, ">=", format_value<T>(value));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator<(T value) const {
			return Condition(_column_name, "<", format_value<T>(value));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator<=(T value) const {
			return Condition(_column_name, "<=", format_value<T>(value));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator!=(T value) const {
			return Condition(_column_name, "!=", format_value<T>(value));
		}

#ifdef SUPPORT_OPTIONAL
		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator==(std::optional<T> value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this == *value;
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator>(std::optional<T> value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this > *value;
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator>=(std::optional<T> value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this >= *value;
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator<(std::optional<T> value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this < *value;
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator<=(std::optional<T> value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this <= *value;
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition operator!=(std::optional<T> value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this != *value;
		}
		/*
		Condition operator==(std::nullopt_t) const {
			return Condition();
		}
		Condition operator!=(std::nullopt_t) const {
			return Condition();
		}
		Condition operator>(std::nullopt_t) const {
			return Condition();
		}
		Condition operator>=(std::nullopt_t) const {
			return Condition();
		}
		Condition operator<(std::nullopt_t) const {
			return Condition();
		}
		Condition operator<=(std::nullopt_t) const {
			return Condition();
		}*/
		//ename LIKE '张%'
		Condition like(const std::optional<std::string>& value, std::string_view fmt = "%{}%") const 
		{
			if (value.has_value()) {
				return like(*value, fmt);
			}
			return Condition();
		}

		template<typename T, std::enable_if_t<(std::is_arithmetic_v<T> || hdy::is_string_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition between_and(const std::optional<T>& lower, const std::optional<T>& upper) const {
			return Condition(_column_name, "BETWEEN", std::format("{} AND {}", format_value(lower), format_value(upper)));
		}

#endif

		//comm IS NULL
		Condition is_null() const {
			return Condition(_column_name, "IS NULL", {});
		}

		//comm IS NOT NULL
		Condition is_not_null() const {
			return Condition(_column_name, "IS NOT NULL", {});
		}

		//ename LIKE '张%'
		Condition like(const std::string& value,std::string_view fmt = "%{}%") const {
			std::string result(fmt.data(), fmt.length());
			if (auto pos = result.find("{}");pos != std::string::npos) {
				result.replace(pos, 2, escape_string(value));
			}
			return Condition(_column_name, "LIKE", "\'" + result + "\'");
		}

		//hiredate BETWEEN '1990-01-01' AND '2000-01-01'
		template<typename T, std::enable_if_t<(std::is_arithmetic_v<T> || hdy::is_string_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Condition between_and(const T& lower, const T& upper) const {
			return Condition(_column_name, "BETWEEN", std::format("{} AND {}", format_value(lower), format_value(upper)));
		}

		//ename IN ('张三', '李四')
		template<typename T, std::enable_if_t<hdy::is_container_v<T> && !hdy::is_string_v<T> && !std::is_arithmetic_v<T>, int> = 0>
		Condition in(const T& value) const {
			return Condition(_column_name, "IN", format_value(value));
		}

		//ename NOT IN ('张三', '李四')
		template<typename T, std::enable_if_t<hdy::is_container_v<T> && !hdy::is_string_v<T> && !std::is_arithmetic_v<T>, int> = 0>
		Condition not_in(const T& value) const {
			return Condition(_column_name, "NOT IN", format_value(value));
		}
	public:	//UPDATE 语句中SET值设置
		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>)
			&& !hdy::is_container_v<T>, int> = 0>
		Set operator=(const T& value) const {
			return Set(_column_name, format_value(value));
		}

		Set operator=(std::nullptr_t) const {
			return Set(_column_name, "NULL");
		}

		Set operator=(const Column& column) const {
			return Set(_column_name, column.name());
		}

#ifdef SUPPORT_OATPP
		template<typename T, std::enable_if_t <is_object_v<T>, int > = 0>
		Set operator=(T& value) const {
			if constexpr (std::is_same_v<T, oatpp::String>) {
				if (value != nullptr && !value->empty()) {
					return Set(_column_name, format_value(*value));
				}
			}
			else {
				if (value != nullptr) {
					return Set(_column_name, format_value(*value));
				}
			}
			return Set();
		}

		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator==(T value) const {
			if (value != nullptr) {
				return *this == *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator!=(T value) const {
			if (value != nullptr) {
				return *this != *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator>(T value) const {
			if (value != nullptr) {
				return *this > *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator>=(T value) const {
			if (value != nullptr) {
				return *this >= *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator<(T value) const {
			if (value != nullptr) {
				return *this < *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator<=(T value) const {
			if (value != nullptr) {
				return *this <= *value;
			}
			return Condition();
		}

		//ename LIKE '张%'
		Condition like(const oatpp::String& value, std::string_view fmt = "%{}%") const 
		{
			if (value != nullptr && !value->empty()) {
				return like(*value, fmt);
			}
			return Condition();
		}

		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition between_and(const T& lower,const T& upper) const {
			if (lower != nullptr && upper != nullptr) {
				bool is_ok = true;
				if constexpr (std::is_same_v<T, oatpp::String>) {
					is_ok = !lower->empty() && !upper->empty();
				}
				if (is_ok) {
					return Condition(_column_name, "BETWEEN", std::format("{} AND {}",format_value(*lower),format_value(*upper)));
				}
			}
			return Condition();
		}
#endif
	public://字段与字段比较
		Condition operator==(const Column& column) const {
			return Condition(_column_name, "=", column.name());
		}

		Condition operator>(const Column& column) const {
			return Condition(_column_name, ">", column.name());
		}

		Condition operator>=(const Column& column) const {
			return Condition(_column_name, ">=", column.name());
		}

		Condition operator<(const Column& column) const {
			return Condition(_column_name, "<", column.name());
		}

		Condition operator<=(const Column& column) const {
			return Condition(_column_name, "<=", column.name());
		}

		Condition operator!=(const Column& column) const {
			return Condition(_column_name, "!=", column.name());
		}

		Condition between_and(const Column& lower, const Column& upper) const {
			return Condition(_column_name, "BETWEEN", std::format("{} AND {}", lower.name(), upper.name()));
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
		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Column operator+(T value) const {
			return Column(std::format("{} + {}", _column_name, format_value(value)));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Column operator-(T value) const {
			return Column(std::format("{} - {}", _column_name, format_value(value)));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Column operator*(T value) const {
			return Column(std::format("{} * {}", _column_name, format_value(value)));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Column operator/(T value) const {
			return Column(std::format("{} / {}", _column_name, format_value(value)));
		}

		template<typename T, std::enable_if_t<(hdy::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::is_container_v<T>, int> = 0>
		Column operator%(T value) const {
			return Column(std::format("{} % {}", _column_name, format_value(value)));
		}
	private:
		std::string _column_name;		/*!字段名*/
		std::string _alias_name;		/*!别名*/
		std::string _ordering;			/*!排序*/
	};

	namespace literals {
		inline Column operator""_c(const char* name, std::size_t size) {
			return Column(std::string(name, size));
		}
	}
	inline std::string format_value(const Column& val) {
		return std::format("{}", static_cast<std::string>(val));
	}

	inline static Column all{ "*" };
	inline static Column count(const Column& column) { return Column(std::format("COUNT({})", column.name())); }
	inline static Column count_all() { return Column("COUNT(*)"); }

	inline static Column max(const Column& column) { return Column(std::format("MAX({})", column.name())); }
	inline static Column min(const Column& column) { return Column(std::format("MIN({})", column.name())); }
	inline static Column avg(const Column& column) { return Column(std::format("AVG({})", column.name())); }
	inline static Column sum(const Column& column) { return Column(std::format("SUM({})", column.name())); }

	//使用逗号连接所有列
	inline std::string join(const std::vector<Column>& columns, const std::string& delimiter = ",")
	{
		std::string result;
		bool first = true;
		for (auto& c : columns) {
			if (!first) {
				result += delimiter;
			}
			result += c.name();
			first = false;
		}
		return result;
	}

	template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && (std::conjunction_v<std::is_same<std::decay_t<Args>, Column>...>), int> = 0>
	inline std::string join(Args&&... args) {
		//解包并格式化值
		return join({ std::forward<Args>(args)... });
	}

	//使用逗号连接所有字符串
	inline std::string join(const std::vector<std::string>& vec, const std::string& delimiter = ",") {
		// 格式化结果
		std::string result;
		bool first = true;
		for (const auto& elem : vec) {
			if (!first) {
				result += delimiter;
			}
			result += elem;
			first = false;
		}
		return result;
	}
}
