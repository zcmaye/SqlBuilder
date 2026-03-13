#pragma once

#include <string>
#include <format>

namespace hdy::tool::sql {

	class Condition {
	public:
		explicit Condition(const std::string& stmt = "")
			: _sql(stmt)
		{
		}

		Condition(const std::string& column, const std::string& op, const std::string& value)
			: _sql(std::format("{} {} {}", column, op, value))
		{
		}

		Condition operator &&(const Condition& other) {
			if (other.empty()) {
				return *this;
			}
			if (empty()) {
				return other;
			}
			return Condition(std::format("({} AND {})", _sql, other._sql));
		}

		Condition operator ||(const Condition& other) {
			if (other.empty()) {
				return *this;
			}
			if (empty()) {
				return other;
			}
			return Condition(std::format("({} OR {})", _sql, other._sql));
		}

		Condition operator !() {
			if (empty()) {
				return *this;;
			}
			return Condition(std::format("NOT {}", _sql));
		}

		bool empty()const { return _sql.empty(); }

		operator const std::string& ()const { return _sql; }
		const std::string& sql()const { return _sql; }
	private:
		std::string _sql;
	};
}
