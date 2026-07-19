#include "Field.h"
#include "SqlBuilder.h"

namespace zc::sqlbuilder {
	Field::Field(const Select& subquery)
		: _name("(" + subquery.to_string() + ")")
	{
	}

	Condition Field::operator==(const Select& subquery) const
	{
		return Condition(_name, "=", "(" + subquery.to_string() + ")");
	}

	Condition Field::operator!=(const Select& subquery) const
	{
		return Condition(_name, "!=", "(" + subquery.to_string() + ")");
	}

	Condition Field::operator>(const Select& subquery) const
	{
		return Condition(_name, ">", "(" + subquery.to_string() + ")");
	}

	Condition Field::operator<(const Select& subquery) const
	{
		return Condition(_name, "<", "(" + subquery.to_string() + ")");
	}

	Condition Field::operator>=(const Select& subquery) const
	{
		return Condition(_name, ">=", "(" + subquery.to_string() + ")");
	}

	Condition Field::operator<=(const Select& subquery) const
	{
		return Condition(_name, "<=", "(" + subquery.to_string() + ")");
	}

	Condition Field::in(const Select& subquery) const
	{
		return Condition(_name, "IN", "(" + subquery.to_string() + ")");
	}

	Condition Field::not_in(const Select& subquery) const
	{
		return Condition(_name, "NOT IN", "(" + subquery.to_string() + ")");
	}
}
