#pragma once

#include "TypeTraits.h"
#include <format>

namespace hdy::tool::sql {

	/**
	 * 定义无效值.
	 */
	inline const std::string InvalidValue { "InvalidValue" };

	/**
	 * 转义字符串参数，防止SQL注入.
	 */
	inline std::string escape_string(const std::string& param) {
		std::string escaped;
		escaped.reserve(param.size() * 2);
		for (char c : param) {
			switch (c) {
			case '\'': escaped += "''"; break;
			case '\\': escaped += "\\\\"; break;
			case '\n': escaped += "\\n"; break;
			case '\r': escaped += "\\r"; break;
			case '\t': escaped += "\\t"; break;
			case '"': escaped += "\\\""; break; // 双引号转义
			//case '%': escaped += "\\%"; break; // LIKE通配符%转义
			//case '_': escaped += "\\_"; break; // LIKE通配符_转义
			default: escaped += c; break;
			}
		}
		return escaped;
	}

	// ========== 第三步：重载format_value函数（区分容器、字符串、算术类型） ==========
	// 1. 字符串类型格式化（单独处理，保留原始字符串，可添加引号增强可读性）
	template <typename T>
	std::string format_value(const T& str,
		std::enable_if_t <hdy::type::traits::is_string_v<T>, int > = 0)
	{
		//转义字符串参数，防止SQL注入
		auto e_str = escape_string(std::format("{}", str));
		// 格式化规则：给字符串添加双引号，增强辨识度
		return std::format("\'{}\'",e_str);
	}

	// 2. 算术类型格式化（直接用std::format）
	template <typename T>
	std::string format_value(const T& val,
		std::enable_if_t<std::is_arithmetic_v<T>, int> = 0)
	{
		return std::format("{}", val);
	}


	// 3. 容器类型格式化（手动拼接元素）
	template <typename Container>
	std::string format_value(const Container& con,
		std::enable_if_t<hdy::type::traits::is_container_v<Container> && !hdy::type::traits::is_map_v<Container>, int> = 0)
	{
		std::string result = "(";
		bool first = true;
		for (const auto& elem : con) {
			if (!first) {
				result += ", ";
			}
			result += format_value(elem); // 递归格式化元素（支持嵌套/算术/字符串元素）
			first = false;
		}
		result += ")";
		return result;
	}

#ifdef SUPPORT_OPTIONAL
	template <typename T>
	std::string format_value(const std::optional<T>& value) {
		if (value.has_value()) {
			return format_value(*value);
		}
		return InvalidValue;
	}
#endif

#ifdef SUPPORT_OATPP
	template <typename T>
	std::string format_value(T& value,
		std::enable_if_t<hdy::type::traits::is_object_v<T>, int> = 0)
	{
		if constexpr (std::is_same_v<T, oatpp::String>) {
			if (value != nullptr && !value->empty()) {
				return format_value(*value);
			}
		}
		else {
			if (value != nullptr) {
				return format_value(*value);
			}
		}
		return InvalidValue;
	}
#endif


	// 4. map/unordered_map/vector<pair<?,?>>类型格式化
	template <typename Container>
	std::string format_value(const Container& con,
		std::enable_if_t<hdy::type::traits::is_map_v<Container>, int> = 0)
	{
		//值类型
		using ValueType = std::decay_t<typename Container::value_type::second_type>;
		std::string result = "{";
		bool first = true;
		for (const auto& elem : con) {
			if (!first) {
				result += ", ";
			}
			if constexpr (hdy::type::traits::is_string_v<ValueType>) {
				result += std::format("\"{}\": \"{}\"", elem.first, elem.second);
			}
			else {
				result += std::format("\"{}\": {}", elem.first, elem.second);
			}
			first = false;
		}
		result += "}";
		return result;
	}

	// 5. 可变参数模板,例如:format_value(1,2,3,"hello");
	template<typename ...Args, std::enable_if_t<(sizeof...(Args) > 1), int> = 0>
		std::string format_value(Args&&... args) {
		//解包并格式化值
		std::vector<std::string> vec = { format_value(std::forward<Args>(args))... };
		// 格式化结果
		std::string result = "(";
		bool first = true;
		for (const auto& elem : vec) {
			if (!first) {
				result += ", ";
			}
			result += elem; //在上面解包中已经格式化了值，直接拼接即可
			first = false;
		}
		result += ")";
		return result;
	}
}

