#include "Table.h"
#include "SqlBuilder.h"


namespace zc::sqlbuilder {
	Table::Table(const Select& subQuery)
		: _name("(" + subQuery.to_string() + ")")
	{
	}
}
