#include "Value.h"
#include "Value.h"
#include "Field.h"

namespace zc::sqlbuilder {

	Value::Value(nullptr_t) 
		: m_value("NULL")
	{

	}

	Value::Value(const Field& field) 
		: m_value(field)
	{
	}

	Value::Value(const std::string& raw_str, Wrap wrap)
		: m_value(wrap == Wrap::Raw ? raw_str : format_value(raw_str))
	{
	}

	std::string ValueList::join(std::string_view delimiter) const{
		// 格式化结果
		std::string result;
		bool first = true;
		for (const auto& elem : *this) {
			if (!first) {
				result += delimiter;
			}
			result += elem;
			first = false;
		}
		return result;
	}

	bool ValueList::contains(std::string_view str, bool caseSensitive)const
	{
		if (caseSensitive) {
			return std::find(begin(), end(), str) != end();
		}
		else {
			// 不区分大小写的查找
			std::string lowerStr;
			lowerStr.reserve(str.length());
			std::transform(str.begin(), str.end(),
				std::back_inserter(lowerStr),
				[](unsigned char c) { return std::tolower(c); });

			return std::any_of(begin(), end(), [&lowerStr](const std::string& item) {
				if (item.length() != lowerStr.length()) {
					return false;
				}
				return std::equal(item.begin(), item.end(), lowerStr.begin(),
					[](char a, char b) {
						return std::tolower(static_cast<unsigned char>(a)) ==
							static_cast<unsigned char>(b);
					});
				});
		}
	}
}
