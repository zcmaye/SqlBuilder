/*****************************************************************//**
 * \file   Table.h
 * \brief  表类
 * 
 * \author Maye
 * \date   July 2026
 *********************************************************************/
#pragma once


#include "SqlException.h"
#include "detail/FormatValue.h"
#include "detail/StringUtils.h"

#include <string_view>
#include <optional>

namespace zc::sqlbuilder {

	class Select;

	class Table {
	public:
		Table() {}

		explicit Table(std::string_view name)
			: _name(name)
		{ }

		Table(const char* name)
			: _name(name)
		{ }

		explicit Table(const Select& subQuery);

		Table& as(const std::string& aname) {
			TABLE_ASSERT(!_alias, "Alias already set");
			_alias = aname;
			return *this;
		}

		const std::string& name() const { return _name; }

		const std::optional<std::string>& alias() const { return _alias; }

		bool has_alias() const { return _alias.has_value(); }

		std::string to_string() const {
			TABLE_ASSERT(!_name.empty(), "Table name is empty");
			if (_alias) {
				return std::format("{} AS '{}' ", _name, *_alias);
			}
			return _name;
		}
		operator bool() const { return !_name.empty(); }
	private:
		std::string _name;
		std::optional<std::string> _alias;
	};

	namespace table_literals {
		inline Table operator""_t(const char* str, size_t len) {
			return Table(std::string(str, len));
		}
		inline Table operator""_table(const char* str, size_t len) {
			return Table(std::string(str, len));
		}
	}
}