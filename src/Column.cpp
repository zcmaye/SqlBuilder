#include "Column.h"
#include "SqlBuilder.h"

namespace hdy::tool::sql {
	Column::Column(const Select& subquery)
		: _column_name("(" + subquery.sql() + ")")
	{
	}

	Condition Column::operator==(const Select& subquery) const
	{
		return Condition(_column_name, "=", "(" + subquery.sql() + ")");
	}

	Condition Column::operator!=(const Select& subquery) const
	{
		return Condition(_column_name, "!=", "(" + subquery.sql() + ")");
	}

	Condition Column::operator>(const Select& subquery) const
	{
		return Condition(_column_name, ">", "(" + subquery.sql() + ")");
	}

	Condition Column::operator<(const Select& subquery) const
	{
		return Condition(_column_name, "<", "(" + subquery.sql() + ")");
	}

	Condition Column::operator>=(const Select& subquery) const
	{
		return Condition(_column_name, ">=", "(" + subquery.sql() + ")");
	}

	Condition Column::operator<=(const Select& subquery) const
	{
		return Condition(_column_name, "<=", "(" + subquery.sql() + ")");
	}

	Condition Column::in(const Select& subquery) const
	{
		return Condition(_column_name, "IN", "(" + subquery.sql() + ")");
	}

	Condition Column::not_in(const Select& subquery) const
	{
		return Condition(_column_name, "NOT IN", "(" + subquery.sql() + ")");
	}
}
