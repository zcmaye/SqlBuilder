#pragma once

#include <string>
#include <format>

namespace hdy::tool::sql {

	/**
	 * UPDATE语句中的SET 字句，支持 "emp.empno"_c = 7788
	 */
	class Set
	{
	public:
		explicit Set(const std::string& stmt = "")
			:_stmt(stmt)
		{
		}

		Set(const std::string& column, const std::string& value)
			:_stmt(std::format("{} = {}", column, value))
		{
		}

		Set operator,(const Set& other)const {
			return Set(*this).concat(other);
		}

		Set& operator+=(const Set& other){
			return concat(other);
		}

		Set& concat(const Set& other){
			if (_stmt.empty()) {
				_stmt = other._stmt;
			}
			else if(other.empty()){
				return *this;
			}
			else {
				_stmt = std::format("{} , {}", _stmt, other._stmt);
			}
			return *this;
		}

		operator const std::string& ()const { return _stmt; }

		std::string sql() const { return _stmt; }

		bool empty()const {return _stmt.empty(); }
	private:
		std::string _stmt;
	};
}
