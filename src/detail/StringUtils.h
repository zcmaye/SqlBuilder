#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace zc::str{
	//使用逗号连接所有字符串
	std::string join(const std::vector<std::string>& vec, const std::string_view& delimiter = ",");
	//判断判断字符串是否包含指定字符串
	bool contains(const std::vector<std::string>& vec, std::string_view str, bool caseSensitive = true);

	//使用指定分隔符分割字符串
	std::vector<std::string> split(const std::string& str, const std::string& sep);
	std::vector<std::string> split(const std::string& str, char sep);

	// 去除字符串首尾空白
	std::string trim(const std::string& str);
	// 字符串转小写
	std::string toLower(const std::string& str);
	// 字符串转大写
	std::string toUpper(const std::string& str);
}
