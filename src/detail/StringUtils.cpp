#include "StringUtils.h"
#include <algorithm>
#include <iterator>

// ============ StringUtils 命名空间函数实现 ============
namespace zc::str{

	std::string join(const std::vector<std::string>& vec, const std::string_view& delimiter) {
		// 格式化结果
		std::string result;
		bool first = true;
		for (const auto& elem : vec) {
			if (!first) {
				result += delimiter;
			}
			result += elem;
			first = false;
		}
		return result;
	}

	bool contains(const std::vector<std::string>& vec, std::string_view str, bool caseSensitive)
    {
        if (caseSensitive) {
			return std::find(vec.begin(), vec.end(), str) != vec.end();
        }
        else {
            // 不区分大小写的查找
            std::string lowerStr;
            lowerStr.reserve(str.length());
            std::transform(str.begin(), str.end(),
                std::back_inserter(lowerStr),
                [](unsigned char c) { return std::tolower(c); });

            return std::any_of(vec.begin(), vec.end(), [&lowerStr](const std::string& item) {
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


    std::vector<std::string> split(const std::string& str, const std::string& sep)
    {
        std::vector<std::string> result;

        if (str.empty()) {
            return result;
        }

        if (sep.empty()) {
            // 如果分隔符为空，将每个字符作为单独的元素
            result.reserve(str.length());
            for (char c : str) {
                result.emplace_back(1, c);
            }
            return result;
        }

        size_t start = 0;
        size_t end = str.find(sep);

        while (end != std::string::npos) {
            result.emplace_back(str.substr(start, end - start));
            start = end + sep.length();
            end = str.find(sep, start);
        }

        // 添加最后一个部分（或空字符串，如果原字符串以分隔符结尾）
        result.emplace_back(str.substr(start));

        return result;
    }

    std::vector<std::string> split(const std::string& str, char sep)
    {
        return split(str, std::string(1, sep));
    }

	// 去除字符串首尾空白
	std::string trim(const std::string& str) {
		size_t first = str.find_first_not_of(" \t\n\r\f\v");
		if (first == std::string::npos) {
			return "";
		}
		size_t last = str.find_last_not_of(" \t\n\r\f\v");
		return str.substr(first, last - first + 1);
	}

	// 字符串转小写
	std::string toLower(const std::string& str) {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return result;
	}

	// 字符串转大写
	std::string toUpper(const std::string& str) {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) { return std::toupper(c); });
		return result;
	}

}