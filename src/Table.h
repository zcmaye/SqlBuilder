#pragma once

#include <string_view>
#include <optional>
#include "FormatValue.h"
#include "SqlException.h"
#include "StringUtils.h"

#define TABLE_ASSERT(cond,msg) if (!(cond)) {throw TableError(msg);}


namespace hdy::tool::sql {

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
	private:
		std::string _name;
		std::optional<std::string> _alias;
	};

	inline Table operator""_t(const char* str, size_t len) {
		return Table(std::string(str, len));
	}
}