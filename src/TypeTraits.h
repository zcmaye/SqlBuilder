#pragma once

#include <type_traits>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

namespace hdy {
	// ========== 第一步：定义容器判定特性（保留原有逻辑，确保排除string） ==========
	template <typename T, typename = void>
	struct is_container : std::false_type {};

	// SFINAE检测容器特征（begin/end/value_type）
	template <typename T>
	struct is_container<
		T,
		std::void_t<
		decltype(std::declval<T>().begin()),
		decltype(std::declval<T>().end()),
		typename T::value_type
		>
	> : std::true_type {
	};

	// 特化匹配initializer_list
	template <typename T>
	struct is_container<std::initializer_list<T>> : std::true_type {};

	// 显式排除std::string（所有basic_string变体）
	template <typename CharT, typename Traits, typename Alloc>
	struct is_container<std::basic_string<CharT, Traits, Alloc>> : std::false_type {};

	template <typename CharT, typename Traits>
	struct is_container<std::basic_string_view<CharT, Traits>> : std::false_type {};

	// 排除字符指针（避免被误判为容器）
	template <>
	struct is_container<const char*> : std::false_type {};
	template <>
	struct is_container<char*> : std::false_type {};

	// 容器特性别名
	template <typename T>
	inline constexpr bool is_container_v = is_container<T>::value;

	// ========== 第二步：定义字符串判定特性 ==========
	template <typename T, typename = void>
	struct is_string : std::false_type {};

	// 匹配std::string（包括wstring等basic_string变体）
	template <typename CharT, typename Traits, typename Alloc>
	struct is_string<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};

	template <typename CharT, typename Traits>
	struct is_string<std::basic_string_view<CharT, Traits>> : std::true_type {};

	// 匹配const char*
	template <>
	struct is_string<const char*> : std::true_type {};

	// 匹配char*
	template <>
	struct is_string<char*> : std::true_type {};

	// 匹配char[N]
	template <size_t N>
	struct is_string<char[N]> : std::true_type {};

	// 字符串特性别名
	template <typename T>
	inline constexpr bool is_string_v = is_string<T>::value;

	// ========== 第三步：定义map判定特性 ==========
	template <typename T, typename = void>
	struct is_map : std::false_type {};

	// 匹配std::map
	template <class _Kty, class _Ty, class _Pr, class _Alloc>
	struct is_map<std::map<_Kty, _Ty, _Pr,_Alloc>> : std::true_type {};

	// 匹配std::unordered_map
	template <class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	struct is_map<std::unordered_map<_Kty, _Ty,_Hasher, _Keyeq,_Alloc>> : std::true_type {};

	// 匹配std::vector<std::pair<?,?>>
	template <class _Kty, class _Vty>
	struct is_map<std::vector<std::pair<_Kty, _Vty>>> : std::true_type {};

	// 字符串特性别名
	template <typename T>
	inline constexpr bool is_map_v = is_map<T>::value;

#ifdef SUPPORT_OATPP
	// ========== 第四步：定义oatpp::Object判定特性 ==========
	template<typename T, typename = void>
	struct is_object : std::false_type {};

	// 特化版本，当T::ObjectType存在时  
	template<typename T>
	struct is_object<T, std::void_t<typename T::ObjectType>> : std::true_type {};

	template<typename T>
	inline constexpr bool is_object_v = is_object<T>::value;
#endif

	template <typename... Args>
	inline constexpr bool is_all_arithmetic_or_string_v =
	std::conjunction_v<std::disjunction<
		std::is_arithmetic<std::decay_t<Args>>, 
		is_string<std::decay_t<Args>>>...>;
		//std::bool_constant<is_string_v<std::decay_t<Args>>> >...>;
}
