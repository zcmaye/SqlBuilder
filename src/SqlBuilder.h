/*****************************************************************//**
 * \file   SqlBuilder.h
 * \brief  核心语句类
 * 
 * \author Maye
 * \date   July 2026
 *********************************************************************/
#pragma once

#include "SqlException.h"
#include "Field.h"
#include "Table.h"
#include <algorithm>

namespace zc::sqlbuilder {
	class Select {
	public:
		/**
		 * 使用N个字段构造Select对象.
		 * 
		 * \param ...args 要查询的字段名:
		 * - 1,查询多个字段，例如:Field("empno"),Field("job")
		 * - 2,查询所有字段，例如:Field("*") 或 Field(all);
		 * - 3,统计行数，	 例如:Field("COUNT(*)") 或 count(all) 或 count_all()
		 * - 4,给字段取别名，例如:Field("empno").as("eno"),Field("job").as("岗位")
		 */
		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && std::conjunction_v<std::is_same<std::decay_t<Args>, Field>...> , int> = 0>
		Select(Args&& ...args)
			: _columns{ std::forward<Args>(args)... }
		{ }

		Select& from(const Table& table) {
			_join_tables.push_back(std::format("FROM {}", table.to_string()));
			return *this;
		}

		Select& inner_join(const Table& table) {
			_join_tables.push_back(std::format("JOIN {}", table.to_string()));
			return *this;
		}

		Select& join(const Table& table) {
			return inner_join(table);
		}

		Select& left_join(const Table& table) {
			_join_tables.push_back(std::format("LEFT JOIN {}", table.to_string()));
			return *this;
		}

		Select& right_join(const Table& table) {
			_join_tables.push_back(std::format("RIGHT JOIN {}", table.to_string()));
			return *this;
		}

		Select& full_join(const Table& table) {
			_join_tables.push_back(std::format("FULL JOIN {}", table.to_string()));
			return *this;
		}

		Select& on(const Condition& cond) {
			_join_tables.push_back(std::format("ON {}", cond.to_string()));
			return *this;
		}

		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && std::conjunction_v<std::is_same<std::decay_t<Args>, Field>...>, int> = 0>
			Select& using_(Args&& ...args) {
			_join_tables.push_back(std::format("USING({})", zc::sqlbuilder::join(args...)));
			return *this;
		}

		Select& as(const std::string& _alias) {
			_join_tables.push_back(std::format("AS '{}'", _alias));
			return *this;
		}

		Select& where(const Condition& cond) {
			_cond = cond;
			return *this;
		}

		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && std::conjunction_v<std::is_same<std::decay_t<Args>, Field>...>, int> = 0>
		Select& group_by(Args&& ...args) {
			_group_by = std::format("GROUP BY {}", zc::sqlbuilder::join(args...));
			return *this;
		}

		Select& having(const Condition& cond) {
			_having = cond;
			return *this;
		}

		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && std::conjunction_v<std::is_same<std::decay_t<Args>, Field>...>, int> = 0>
			Select& order_by(Args&& ...args) {
			_order_by = std::format("ORDER BY {}", zc::sqlbuilder::join(args...));
			return *this;
		}

		Select& limit(size_t offset, size_t count) {
			_limit = std::format("LIMIT {} OFFSET {}", count, offset);
			return *this;
		}

		Select& limit(size_t count) {
			_limit = std::format("LIMIT {}", count);
			return *this;
		}

		std::string to_string()const {
			ZC_ASSERT(!_columns.empty(),"must have a column!");
			ZC_ASSERT(!_join_tables.empty(),"must have a table!");

			std::string result;
			result.reserve(256);	//预估容量
			result = std::format("SELECT {}", zc::sqlbuilder::join(_columns,", "));
			result += ' ';
			result += _join_tables.join(" ");
			if (!_cond.empty()) {
				result += ' ';
				result += std::format("WHERE {}", _cond.to_string());
			}
			if (!_group_by.empty()) {
				result += ' ';
				result += _group_by;
				if (!_having.empty()) {
					result += ' ';
					result += std::format("HAVING {}", _having.to_string());
				}
			}
			if (!_order_by.empty()) {
				result += ' ';
				result += _order_by;
			}
			if (!_limit.empty()) {
				result += ' ';
				result += _limit;
			}
			return result;
		}
	public://子查询
#ifdef USE_TEMPLATE
		template<typename T, std::enable_if_t<!std::is_same_v<T, Field>&& std::is_same_v<T, Select>, int> = 0>
		Select& from(const T& subquery) {
			_join_tables.push_back(std::format("FROM ({})", subquery.to_string()));
			return *this;
		}

		template<typename T, std::enable_if_t<!std::is_same_v<T, Field>&& std::is_same_v<T, Select>, int> = 0>
		Select& inner_join(const T& subquery) {
			_join_tables.push_back(std::format("JOIN ({})", subquery.to_string()));
			return *this;
		}

		template<typename T, std::enable_if_t<!std::is_same_v<T, Field>&& std::is_same_v<T, Select>, int> = 0>
		Select& join(const T& subquery) {
			return inner_join(subquery);
		}

		template<typename T, std::enable_if_t<!std::is_same_v<T, Field>&& std::is_same_v<T, Select>, int> = 0>
		Select& left_join(const T& subquery) {
			_join_tables.push_back(std::format("LEFT JOIN ({})", subquery.to_string()));
			return *this;
		}

		template<typename T, std::enable_if_t<!std::is_same_v<T, Field>&& std::is_same_v<T, Select>, int> = 0>
		Select& right_join(const T& subquery) {
			_join_tables.push_back(std::format("RIGHT JOIN ({})", subquery.to_string()));
			return *this;
		}

		template<typename T, std::enable_if_t<!std::is_same_v<T, Field>&& std::is_same_v<T, Select>, int> = 0>
		Select& full_join(const T& subquery) {
			_join_tables.push_back(std::format("FULL JOIN ({})", subquery.to_string()));
			return *this;
		}
#else
		Select& from(const Select& subquery) {
			_join_tables.push_back(std::format("FROM ({})", subquery.to_string()));
			return *this;
		}

		Select& inner_join(const Select& subquery) {
			_join_tables.push_back(std::format("JOIN ({})", subquery.to_string()));
			return *this;
		}

		Select& join(const Select& subquery) {
			return inner_join(subquery);
		}

		Select& left_join(const Select& subquery) {
			_join_tables.push_back(std::format("LEFT JOIN ({})", subquery.to_string()));
			return *this;
		}

		Select& right_join(const Select& subquery) {
			_join_tables.push_back(std::format("RIGHT JOIN ({})", subquery.to_string()));
			return *this;
		}

		Select& full_join(const Select& subquery) {
			_join_tables.push_back(std::format("FULL JOIN ({})", subquery.to_string()));
			return *this;
		}
#endif
	private:
		std::vector<Field> _columns;
		StringList _join_tables;
		Condition _cond;
		std::string _group_by;
		Condition _having;
		std::string _order_by;
		std::string _limit;
	};

	class Insert {
	public:
		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && (std::conjunction_v<std::is_same<std::decay_t<Args>, Field>...>), int> = 0>
			Insert(Args&& ...args)
			: _columns({ std::forward<Args>(args)... })
		{
		}

		Insert& into(const Table& table) {
			_table = table;
			return *this;
		}

		template<typename ...Args, std::enable_if_t < (sizeof...(Args) > 0)/* && is_all_arithmetic_or_string_v<Args...>*/, int> = 0>
		Insert& values(Args&& ...args) {
			ZC_ASSERT(_columns.size() == sizeof...(args), "fields and values size not match");
			auto vec = StringList{ format_value(args)... };
			_values.emplace_back(vec);
			return *this;
		}

		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0)/* && is_all_arithmetic_or_string_v<Args...>*/, int> = 0>
		Insert& operator()(Args&& ...args) {
			return values(std::forward<Args>(args)...);
		}

		template<typename Object, typename Fn /* = std::function<StringList(Object)> */,
			std::enable_if_t<!zc::type::traits::is_container_v<std::decay_t<Object>>, int> = 0>
		Insert& values_if(Object&& object, Fn&& fn) {
			auto vec = fn(object);
			ZC_ASSERT(_columns.size() == vec.size(), "fields and values size not match");
			_values.emplace_back(vec);
			return *this;
		}

		template<typename Container, typename Fn /* = std::function<StringList(Object)> */,
			std::enable_if_t<zc::type::traits::is_container_v<std::decay_t<Container>>, int> = 0>
		Insert& values_for(Container&& con, Fn&& fn) {
			for (auto& object : con) {
				auto vec = fn(object);
				ZC_ASSERT(_columns.size() == vec.size(), "fields and values size not match");
				_values.emplace_back(vec);
			}
			return *this;
		}

		std::string to_string()const {
			ZC_ASSERT(!_columns.empty() && !_values.empty(), "fields or values_list is empty");
			ZC_ASSERT(_table, "table is empty");
			auto skips = skip_columns();
			return std::format("INSERT INTO {}({}) VALUES {}", _table.to_string(), field_string(skips), value_string(skips));
		}

	private:
		/**
		 * 根据无效值获取跳过的列.
		 * 
		 * \return 
		 */
		std::vector<int> skip_columns()const {
			std::vector<int> skip_columns_;
			auto& value = _values.front();
			for (size_t i = 0; i < value.size(); i++) {
				if (value[i] == InvalidValue) {
					skip_columns_.push_back(i);
				}
			}
			return skip_columns_;
		}

		/**
		 * 生成字段列表(filed1,file2,...).
		 */
		std::string field_string(const std::vector<int>& skips) const {
			std::string result;
			bool first = true;
			for (size_t i = 0; i < _columns.size(); ++i) {
				if (std::binary_search(skips.begin(), skips.end(), i)) continue;
				if (!first) result += ", ";
				result += _columns[i].name();
				first = false;
			}
			return result;
		}

		/**
		 * 生成值列表(value1,value2,...),(value1,value2,...)...
		 */
		std::string value_string(const std::vector<int>& skips) const {
			std::string result;
			bool first_row = true;
			for (auto& value : _values) {
				if (!first_row) result += ", ";
				result += "(";
				bool first_col = true;
				for (size_t i = 0; i < value.size(); ++i) {
					if (std::binary_search(skips.begin(), skips.end(), i)) continue;
					if (!first_col) result += ", ";
					result += value[i];
					first_col = false;
				}
				result += ")";
				first_row = false;
			}
			return result;
		}
	private:
		Table _table;
		std::vector<Field> _columns;
		std::vector<StringList> _values;
	};

	class Update {
	public:
		Update(const Table& table)
			: _table(table)
		{
		}

		Update& set(const AssignmentList& set) {
			_sets += set;
			return *this;
		}

		Update& operator()(const AssignmentList& _set) {
			return set(_set);
		}

		Update& where(const Condition& condition) {
			_condition = condition;
			return *this;
		}

		std::string to_string()const {
			ZC_ASSERT(_table, "table is empty");
			ZC_ASSERT(_sets, "sets is empty");
			ZC_ASSERT(!_condition.empty(), "condition is empty");
			return std::format("UPDATE {} SET {} WHERE {}", _table.to_string(), _sets.to_string(), _condition.to_string());
		}
	private:
		Table _table;
		AssignmentList _sets;
		Condition _condition;
	};

	class Delete {
	public:
		Delete(const Table& table)
			: _table(table)
		{
		}

		Delete& where(const Condition& condition) {
			_condition = condition;
			return *this;
		}

		std::string to_string()const {
			ZC_ASSERT(_table, "table is empty");
			ZC_ASSERT(!_condition.empty(), "condition is empty");

			auto result = std::format("DELETE FROM {} WHERE {} ", _table.to_string(), _condition.to_string());
			return result;
		}
	private:
		Table _table;
		Condition _condition;
	};
}
