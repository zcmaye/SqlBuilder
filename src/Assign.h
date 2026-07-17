#pragma once

#include <string>
#include <format>

namespace hdy::tool::sql {

	/**
	 * UPDATE语句中的SET 字句，支持 "emp.empno"_c = 7788
	 */
#if 0
	class Assign
	{
	public:
		explicit Assign(const std::string& stmt = "")
			:_stmt(stmt)
		{
		}

		Assign(const std::string& column, const std::string& value)
			:_stmt(std::format("{} = {}", column, value))
		{
		}

		Assign operator,(const Assign& other)const {
			return Assign(*this).concat(other);
		}

		Assign& operator+=(const Assign& other){
			return concat(other);
		}

		Assign& concat(const Assign& other){
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

		std::string to_string() const { return _stmt; }

		bool empty()const {return _stmt.empty(); }
	private:
		std::string _stmt;
	};
	using AssignmentList = Assign;
#else
	class Assign
	{
	public:
		explicit Assign() { }

		Assign(std::string field, std::string value)
			: _field(std::move(field)), _value(std::move(value))
		{ }

		std::string to_string() const {
			return std::format("{} = {}", _field, _value);
		}
		operator const std::string ()const { return to_string(); }

		bool empty()const { return _value.empty(); }
	private:
		std::string _field;
		std::string _value;
	};

	class AssignmentList {
	public:
		explicit AssignmentList() {}

		AssignmentList(std::string field, std::string value)
			: _sets({ Assign{std::move(field), std::move(value)} })
		{ }

		AssignmentList(Assign set)
			: _sets({ std::move(set)})
		{ }

		AssignmentList& operator+=(const AssignmentList& other) {
			return add(other);
		}

		AssignmentList& add(const AssignmentList& other) {
			_sets.insert(_sets.end(), other._sets.begin(), other._sets.end());
			return *this;
		}

		AssignmentList& add(const Assign& set) {
			_sets.push_back(set);
			return *this;
		}

		AssignmentList& add(Assign&& set) {
			_sets.emplace_back(std::move(set));
			return *this;
		}

		operator const std::string ()const { return to_string(); }

		std::string to_string() const {
			std::string result;
			bool first = true;
			for (auto& s : _sets) {
				if (!first) {
					result += ", ";
				}
				result.append(s.to_string());
				first = false;
			}
			return result;
		}

		bool empty()const { return _sets.empty(); }
	private:
		std::vector<Assign> _sets;
	};

	inline AssignmentList operator,(const Assign& left, const Assign& right) {
		return AssignmentList().add(left).add(right);
	}
	inline	AssignmentList operator,(const AssignmentList& left, const AssignmentList& right){
		return AssignmentList(left).add(right);
	}
	inline	AssignmentList operator,(const AssignmentList& sets, const Assign& set){
		return AssignmentList(set).add(set);
	}
	inline	AssignmentList operator,(const Assign& set, const AssignmentList& sets){
		return AssignmentList(set).add(sets);
	}
#endif
	}
