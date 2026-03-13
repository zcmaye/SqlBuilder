#pragma once

#include "Column.h"
#include <algorithm>
#include <sstream>

namespace hdy::tool::sql {

	class Select {
	public:
		/**
		 * 使用N个字段构造Select对象.
		 * 
		 * \param ...args 要查询的字段名:
		 * - 1,查询多个字段，例如:Column("empno"),Column("job")
		 * - 2,查询所有字段，例如:Column("*") 或 Column(all);
		 * - 3,统计行数，	 例如:Column("COUNT(*)") 或 count(all) 或 count_all()
		 * - 4,给字段取别名，例如:Column("empno").as("eno"),Column("job").as("岗位")
		 */
		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && std::conjunction_v<std::is_same<std::decay_t<Args>, Column>...> , int> = 0>
		Select(Args&& ...args)
			: _columns{ std::forward<Args>(args)... }
		{ }

		Select& from(const std::string& table) {
			_join_tables.push_back(std::format("FROM {}", table));
			return *this;
		}

		Select& inner_join(const std::string& table) {
			_join_tables.push_back(std::format("JOIN {}", table));
			return *this;
		}

		Select& join(const std::string& table) {
			return inner_join(table);
		}

		Select& left_join(const std::string& table) {
			_join_tables.push_back(std::format("LEFT JOIN {}", table));
			return *this;
		}

		Select& right_join(const std::string& table) {
			_join_tables.push_back(std::format("RIGHT JOIN {}", table));
			return *this;
		}

		Select& full_join(const std::string& table) {
			_join_tables.push_back(std::format("FULL JOIN {}", table));
			return *this;
		}

		Select& on(const Condition& cond) {
			_join_tables.push_back(std::format("ON {}", cond.sql()));
			return *this;
		}

		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && std::conjunction_v<std::is_same<std::decay_t<Args>, Column>...>, int> = 0>
			Select& using_(Args&& ...args) {
			_join_tables.push_back(std::format("USING({})", hdy::tool::sql::join(args...)));
			return *this;
		}

		Select& as(const std::string& _alias) {
			_join_tables.push_back(std::format("AS {}", _alias));
			return *this;
		}

		Select& where(const Condition& cond) {
			_cond = cond;
			return *this;
		}

		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && std::conjunction_v<std::is_same<std::decay_t<Args>, Column>...>, int> = 0>
		Select& group_by(Args&& ...args) {
			_group_by = std::format("GROUP BY {}", hdy::tool::sql::join(args...));
			return *this;
		}

		Select& having(const Condition& cond) {
			_having = cond;
			return *this;
		}

		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && std::conjunction_v<std::is_same<std::decay_t<Args>, Column>...>, int> = 0>
			Select& order_by(Args&& ...args) {
			_order_by = std::format("ORDER BY {}", hdy::tool::sql::join(args...));
			return *this;
		}

		Select& limit(size_t offset, size_t count) {
			_limit = std::format("LIMIT {},{}", offset, count);
			return *this;
		}

		Select& limit(size_t count) {
			_limit = std::format("LIMIT {}", count);
			return *this;
		}

		std::string sql()const {
			if (_columns.empty()) {
				throw std::runtime_error("must have a column");
			}
			if (_join_tables.empty()) {
				throw std::runtime_error("must have a table");
			}

			std::string result;
			result = std::format("SELECT {}", hdy::tool::sql::join(_columns));
			result += " " + hdy::tool::sql::join(_join_tables, " ");
			if (!_cond.empty()) {
				result += " " + std::format("WHERE {}", _cond.sql());
			}
			if (!_group_by.empty()) {
				result += " " + _group_by;
				if (!_having.empty()) {
					result += " " + std::format("HAVING {}", _having.sql());
				}
			}
			if (!_order_by.empty()) {
				result += " " + _order_by;
			}
			if (!_limit.empty()) {
				result += " " + _limit;
			}
			return result;
		}
	public://子查询
		template<typename T,std::enable_if_t<!std::is_same_v<T,Column> && std::is_same_v<T,Select>,int> = 0>
		Select& from(const T& subquery) {
			_join_tables.push_back(std::format("FROM ({})", subquery.sql()));
			return *this;
		}

		template<typename T,std::enable_if_t<!std::is_same_v<T,Column> && std::is_same_v<T,Select>,int> = 0>
		Select& inner_join(const T& subquery) {
			_join_tables.push_back(std::format("JOIN ({})", subquery.sql()));
			return *this;
		}

		template<typename T,std::enable_if_t<!std::is_same_v<T,Column> && std::is_same_v<T,Select>,int> = 0>
		Select& join(const T& subquery) {
			return inner_join(subquery);
		}

		template<typename T,std::enable_if_t<!std::is_same_v<T,Column> && std::is_same_v<T,Select>,int> = 0>
		Select& left_join(const T& subquery) {
			_join_tables.push_back(std::format("LEFT JOIN ({})", subquery.sql()));
			return *this;
		}

		template<typename T,std::enable_if_t<!std::is_same_v<T,Column> && std::is_same_v<T,Select>,int> = 0>
		Select& right_join(const T& subquery) {
			_join_tables.push_back(std::format("RIGHT JOIN ({})", subquery.sql()));
			return *this;
		}

		template<typename T,std::enable_if_t<!std::is_same_v<T,Column> && std::is_same_v<T,Select>,int> = 0>
		Select& full_join(const T& subquery) {
			_join_tables.push_back(std::format("FULL JOIN ({})", subquery.sql()));
			return *this;
		}
	private:
		std::vector<Column> _columns;
		std::vector<std::string> _join_tables;
		Condition _cond;
		std::string _group_by;
		Condition _having;
		std::string _order_by;
		std::string _limit;
	};

	class Insert {
	public:
		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0) && (std::conjunction_v<std::is_same<std::decay_t<Args>, Column>...>), int> = 0>
			Insert(Args&& ...args)
			: _columns({ std::forward<Args>(args)... })
		{
		}

		Insert& into(const std::string& table) {
			_table = table;
			return *this;
		}

		template<typename ...Args, std::enable_if_t < (sizeof...(Args) > 0)/* && is_all_arithmetic_or_string_v<Args...>*/ , int> = 0>
		Insert& values(Args&& ...args) {
			if (_columns.size() != sizeof...(args)) {
				throw std::runtime_error("fields and values size not match");
			}
			auto vec = std::vector<std::string>{ format_value(args)... };
			_values.emplace_back(vec);
			return *this;
		}

		template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 0)/* && is_all_arithmetic_or_string_v<Args...>*/, int> = 0>
		Insert& operator()(Args&& ...args) {
			return values(std::forward<Args>(args)...);
		}

		template<typename Object, typename Fn /* = std::function<std::vector<std::string>(Object)> */,
			std::enable_if_t<!hdy::is_container_v<std::decay_t<Object>>, int> = 0>
		Insert& values_if(Object&& object, Fn&& fn) {
			auto vec = fn(object);
			if (_columns.size() != vec.size()) {
				throw std::runtime_error("fields and values size not match");
			}
			_values.emplace_back(vec);
			return *this;
		}

		template<typename Container, typename Fn /* = std::function<std::vector<std::string>(Object)> */,
			std::enable_if_t<hdy::is_container_v<std::decay_t<Container>>, int> = 0>
		Insert& values_for(Container&& con, Fn&& fn) {
			for (auto& object : con) {
				auto vec = fn(object);
				if (_columns.size() != vec.size()) {
					throw std::runtime_error("fields and values size not match");
				}
				_values.emplace_back(vec);
			}
			return *this;
		}

		std::string sql()const {
			if (_columns.empty() || _values.empty()) {
				throw std::runtime_error("fields or values_list is empty");
			}
			if (_table.empty()) {
				throw std::runtime_error("table is empty");
			}

			auto skips = skip_columns();
			std::stringstream ss;
			ss << _insert << " " << _table << "(" << field_string(skips) << ") VALUES " << value_string(skips);
			return ss.str();
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
		std::string field_string(const std::vector<int>& skips)const {
			const std::string delimiter = " , ";
			std::string str;
			for(size_t i = 0; i < _columns.size(); i++) {
				if (!std::binary_search(skips.begin(), skips.end(), i)) {
					str += _columns[i].name();
					if (i < _columns.size() - 1) {
						str += delimiter;
					}
				}
				else {
					printf("[field_string]: skip column %s\n", _columns[i].name().c_str());
				}
			}
			//删掉最后的分隔符
			if (str.ends_with(delimiter)) {
				str.resize(str.size() - delimiter.size());
			}
			return str;
		}

		/**
		 * 生成值列表(value1,value2,...),(value1,value2,...)...
		 */
		std::string value_string(const std::vector<int>& skips)const {
			const std::string delimiter = " , ";
			std::string str;

			bool first = true;
			for (auto& value : _values) {
				if (!first) {
					str += delimiter;
				}
				str += "(";
				for (size_t i = 0; i < value.size(); i++) {
					if (!std::binary_search(skips.begin(), skips.end(), i)) {
						str += value[i];
						if (i < value.size() - 1) {
							str += delimiter;
						}
					}
				}
				//删掉最后的分隔符
				if (str.ends_with(delimiter)) {
					str.resize(str.size() - delimiter.size());
				}
				str += ")";
				first = false;
			}
			return str;
		}
	private:
		std::string _insert{ "INSERT INTO" };
		std::vector<Column> _columns;
		std::vector<std::vector<std::string>> _values;
		std::string _table;
	};

	class Update {
	public:
		Update(const std::string& table_name)
			: _table(table_name)
		{
		}

		Update& set(const Set& set) {
			_set += set;
			return *this;
		}

		Update& operator()(const Set& _set) {
			return set(_set);
		}

		Update& where(const Condition& condition) {
			_condition = condition;
			return *this;
		}

		std::string sql()const {
			if (_condition.empty()) {
				throw std::runtime_error("condition is empty");
			}

			std::string result;
			result = std::format("UPDATE {} SET {} WHERE {}", _table, _set.sql(), _condition.sql());
			auto con = _condition.sql();
			return result;
		}
	private:
		std::string _table;
		Set _set;
		Condition _condition;
	};

	class Delete {
	public:
		Delete(const std::string& table_name)
			: _table(table_name)
		{
		}

		Delete& where(const Condition& condition) {
			_condition = condition;
			return *this;
		}

		std::string sql()const {
			if (_condition.empty()) {
				throw std::runtime_error("condition is empty");
			}
			std::string result;
			result = std::format("DELETE FROM {} WHERE {} ", _table, _condition.sql());
			return result;
		}
	private:
		std::string _table;
		Condition _condition;
	};

}
