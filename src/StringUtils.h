#pragma once

#include <string>
#include <vector>
#include <algorithm>

class StringList : public std::vector<std::string>
{
public:
	using std::vector<std::string>::vector;

	std::string join(const std::string& sep) const;
	std::string join(char sep) const;

	bool contains(const std::string& str, bool caseSensitive = true) const;
	bool contains(std::string_view str, bool caseSensitive = true) const;
};


namespace StringUtils {
	StringList split(const std::string& str, const std::string& sep);
	StringList split(const std::string& str, char sep);

	// 去除字符串首尾空白
	std::string trim(const std::string& str);
	// 字符串转小写
	std::string toLower(const std::string& str);
	// 字符串转大写
	std::string toUpper(const std::string& str);
}
