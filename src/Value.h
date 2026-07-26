/*****************************************************************//**
 * \file   Value.h
 * \brief  SQL VALUE
 * 
 * \author Maye
 * \date   July 2026
 *********************************************************************/
#pragma once

#include "detail/FormatValue.h"

namespace zc::sqlbuilder {
    class Field;

	class Value {
    public:
        enum Wrap{
            Raw,
            Quote
        };
	public:
		Value() = default;

        Value(nullptr_t);

        Value(const Field& field);

		Value(const std::string& raw_str, Wrap wrap);

		template <typename T>
		Value(const T& value)
			: m_value(format_value(value))
		{
		}

		const std::string& value() const { return m_value; }
		bool empty()const { return m_value.empty(); }

		operator const std::string& ()const { return m_value; }
		operator std::string_view ()const { return m_value; }
	private:
		std::string m_value;
	};
	const Value null{ nullptr };
	const Value not_null{ "NOT NULL",Value::Raw };

	inline std::string& operator+=(std::string& str, const Value& value) {
		str += value.value();
		return str;
	}

	inline bool operator==(const std::string& str, const Value& value) {
		return str == value.value();
	}

	//using ValueList = std::vector<Value>;
    class ValueList : public std::vector<Value>
    {
    public:
        using std::vector<Value>::vector;

		template<typename T, std::enable_if_t<zc::type::traits::is_container_v<T>, int> = 0>
        ValueList(const T& container) {
            for (auto& v : container) {
				push_back(Value(v));
            }
        }

        std::string to_string()const {
            return join();
        }
		operator std::string ()const { return to_string(); }

		//使用逗号连接所有字符串
		std::string join(std::string_view delimiter = ",") const;
		//判断判断字符串是否包含指定字符串
		bool contains(std::string_view str, bool caseSensitive = true) const;

    };

}

// 高效格式化：直接使用 string_view
template <>
struct std::formatter<zc::sqlbuilder::Value> : std::formatter<std::string_view> {
	// 直接使用基类的 parse

	auto format(const zc::sqlbuilder::Value& v, std::format_context& ctx) const {
		// 使用 string_view 避免拷贝
		return std::formatter<std::string_view>::format(
			static_cast<std::string_view>(v), ctx
		);
	}
};

#if 0
// 为 vector<T> 特化 formatter
template <typename T>
struct std::formatter<std::vector<T>> {
    // 默认格式：使用逗号分隔
    char m_delimiter = ',';
    char m_open = '[';
    char m_close = ']';
    bool m_show_brackets = true;

    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();

        // 解析自定义格式：可以添加选项
        // 例如：{:d} 使用分号分隔，{:n} 不显示括号
        if (it != end && *it == 'd') {
            m_delimiter = ';';
            ++it;
        } else if (it != end && *it == 'n') {
            m_show_brackets = false;
            ++it;
        }

        return it;
    }

    auto format(const std::vector<T>& vec, std::format_context& ctx) const {
        std::string result;

        if (m_show_brackets) {
            result += m_open;
        }

        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                result += m_delimiter;
                result += ' ';
            }
            // 使用 std::format 格式化每个元素
            result += std::format("{}", vec[i]);
        }

        if (m_show_brackets) {
            result += m_close;
        }

        return std::format_to(ctx.out(), "{}", result);
    }
};
#else
template <>
struct std::formatter<zc::sqlbuilder::ValueList> {
    // 默认格式：使用逗号分隔
    char m_delimiter = ',';
    char m_open = '[';
    char m_close = ']';
    bool m_show_brackets = true;

    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();

        // 解析自定义格式：可以添加选项
        // 例如：{:d} 使用分号分隔，{:n} 不显示括号
        if (it != end && *it == 'd') {
            m_delimiter = ';';
            ++it;
        } else if (it != end && *it == 'n') {
            m_show_brackets = false;
            ++it;
        }

        return it;
    }

    auto format(const zc::sqlbuilder::ValueList& vec, std::format_context& ctx) const {
        std::string result;

        if (m_show_brackets) {
            result += m_open;
        }

        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                result += m_delimiter;
                result += ' ';
            }
            // 使用 std::format 格式化每个元素
            result += std::format("{}", vec[i]);
        }

        if (m_show_brackets) {
            result += m_close;
        }

        return std::format_to(ctx.out(), "{}", result);
    }
};

#endif
