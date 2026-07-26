/*****************************************************************//**
 * \file   Assign.h
 * \brief  赋值类(支持 "emp.empno"_f = 7788)
 * 
 * \author Maye
 * \date   July 2026
 *********************************************************************/
#pragma once

#include "SqlException.h"
#include "Value.h"
#include <format>
#include <vector>

namespace zc::sqlbuilder {
	/**
	 * 单个字段赋值.
	 */
#if 0
	class Assign
	{
	public:
		explicit Assign() {}
		Assign(std::string field, std::string value)
			: _field(std::move(field)), _value(std::move(value))
		{
		}

		std::string to_string() const {
			ZC_ASSERT(!_field.empty() && !_value.empty(), "field or value is empty");
			return std::format("{} = {}", _field, _value);
		}
		operator const std::string()const { return to_string(); }

		bool empty()const { return _value.empty(); }
		explicit operator bool()const { return !empty(); }
	private:
		std::string _field;
		std::string _value;
	};
#else
	class Assign
	{
	public:
		explicit Assign() {}
		Assign(std::string field, Value value)
			: _field(std::move(field)), _value(std::move(value))
		{
		}

		std::string to_string() const {
			ZC_ASSERT(!_field.empty() && !_value.empty(), "field or value is empty");
			return std::format("{} = {}", _field, _value);
		}
		operator const std::string()const { return to_string(); }

		bool empty()const { return _value.empty() || _field.empty(); }
		explicit operator bool()const { return !empty(); }
	private:
		std::string _field;
		Value _value;
	};

#endif

	class AssignmentList {
	public:
		explicit AssignmentList() {}

		AssignmentList(std::string field, Value value)
		{
			add(Assign(std::move(field), std::move(value)));
		}

		AssignmentList(Assign set)
		{
			add(std::move(set));
		}

		AssignmentList& operator+=(const AssignmentList& other) {
			return add(other);
		}

		AssignmentList& add(const AssignmentList& other) {
			if (!other.empty()) {
				_sets.insert(_sets.end(), other._sets.begin(), other._sets.end());
			}
			return *this;
		}

		AssignmentList& add(const Assign& set) {
			if (!set.empty()) {
				_sets.push_back(set);
			}
			return *this;
		}

		AssignmentList& add(Assign&& set) {
			if (!set.empty()) {
				_sets.emplace_back(std::move(set));
			}
			return *this;
		}

		operator const std::string()const { return to_string(); }

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
		explicit operator bool()const { return !empty(); }
	private:
		std::vector<Assign> _sets;
	};

	inline AssignmentList operator,(const Assign& left, const Assign& right) {
		return AssignmentList().add(left).add(right);
	}
	inline	AssignmentList operator,(const AssignmentList& left, const AssignmentList& right) {
		return AssignmentList(left).add(right);
	}
	inline	AssignmentList operator,(const AssignmentList& sets, const Assign& set) {
		return AssignmentList(sets).add(set);
	}
	inline	AssignmentList operator,(const Assign& set, const AssignmentList& sets) {
		return AssignmentList(set).add(sets);
	}
}
