#pragma once

#include <string>
#include <format>

namespace zc::sqlbuilder {

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

		Condition operator &&(const Condition& other) const{
			if (other.empty()) {
				return *this;
			}
			if (empty()) {
				return other;
			}
			return Condition(std::format("({} AND {})", _sql, other._sql));
		}

		Condition operator ||(const Condition& other) const{
			if (other.empty()) {
				return *this;
			}
			if (empty()) {
				return other;
			}
			return Condition(std::format("({} OR {})", _sql, other._sql));
		}

		Condition operator!() const {
			if (empty()) {
				return *this;
			}
			return Condition(std::format("NOT {}", _sql));
		}

		operator const std::string &() const { return _sql; }
		const std::string& to_string()const { return _sql; }

		bool empty() const { return _sql.empty(); }
	private:
		std::string _sql;
	};
}
