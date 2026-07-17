#include "Table.h"
#include "SqlBuilder.h"


namespace hdy::tool::sql {
	Table::Table(const Select& subQuery)
		: _name("(" + subQuery.to_string() + ")")
	{
	}
}
