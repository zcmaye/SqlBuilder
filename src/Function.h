/*****************************************************************//**
 * \file   Function.h
 * \brief  SQL函数
 * 
 * \author Maye
 * \date   July 2026
 *********************************************************************/
#pragma once

#include "Field.h"

namespace zc::sqlbuilder::fun{

	/**
	 * 返回原始字符串，不使用单引号包裹.
	 */
	inline Field raw(const std::string& str) {
		return Field(str);
	}

		/**
	 * 生成(CASE <value> WHEN <when> THEN <then> ... END)语句.
	 */
	template<typename Value,typename Whens, typename Thens, std::enable_if_t<zc::type::traits::is_container_v<Whens>&& zc::type::traits::is_container_v<Thens>, int> = 0>
	inline Field case_end(Value value, const Whens& when, const Thens& then) {
		//static_assert(std::is_same_v<Value, typename Whens::value_type>,"value type not equal when type");
		if (when.size() != then.size()) {
			throw std::runtime_error("when and then size not equal");
		}
		std::string result ="(CASE " + format_value(value) + " ";
		for (size_t i = 0; i < when.size(); i++) {
			result += std::format(" WHEN {} THEN {} ", format_value(when[i]), format_value(then[i]));
		}
		return Field(result + "END)");
	}

	template<typename Value,typename ObjectList, typename Fn, std::enable_if_t<zc::type::traits::is_container_v<ObjectList>, int> = 0>
	inline Field case_end_for(Value value, const ObjectList& objects, Fn&& fn) {
		if (objects.empty()) {
			throw std::runtime_error("objects is empty");
		}
		std::string result = "(CASE " + format_value(value) + " ";
		for (auto& object : objects) { 
			std::pair<std::string, std::string> pair = fn(object);
			result += std::format(" WHEN {} THEN {} ", pair.first, pair.second);
		}
		return Field(result + "END)");
	}

	/***************************************聚合函数**********************************************/
	inline static Field count(const Field& field) { return Field(std::format("COUNT({})", field.name())); }
	inline static Field count_all() { return Field("COUNT(*)"); }

	inline static Field max(const Field& field) { return Field(std::format("MAX({})", field.name())); }
	inline static Field min(const Field& field) { return Field(std::format("MIN({})", field.name())); }
	inline static Field avg(const Field& field) { return Field(std::format("AVG({})", field.name())); }
	inline static Field sum(const Field& field) { return Field(std::format("SUM({})", field.name())); }


	/***************************************常用函数**********************************************/
	template<typename T, std::enable_if_t<zc::type::traits::is_string_v<T> || std::is_same_v<T, Field>, int> = 0>
	inline Field upper(const T& v) {
		return Field(std::format("UPPER({})",format_value(v)));
	}
	inline Field ucase(std::string_view v) {
		return upper(v);
	}
	inline Field lower(std::string_view v) {
		return Field(std::format("LOWER('{}')", v));
	}
	inline Field lcase(std::string_view v) {
		return lower(v);
	}
	inline Field replace(std::string_view str,std::string_view newStr) {
		return Field(std::format("REPLACE('{}','{}')", str,newStr));
	}
	inline Field insert(std::string_view str,size_t idx,size_t len,std::string_view newStr) {
		return Field(std::format("INSERT('{}',{},{},'{}')", str, idx, len, newStr));
	}
	inline Field length(std::string_view str) {
		return Field(std::format("LENGTH('{}')", str));
	}
	inline Field char_length(std::string_view str) {
		return Field(std::format("CHAR_LENGTH('{}')", str));
	}
	inline Field substr(std::string_view str, size_t idx, size_t len = -1) {
		if (len == -1)
			return Field(std::format("SUBSTR('{}',{})", str, idx));
		return Field(std::format("SUBSTR('{}',{},{})", str, idx, len));
	}
	inline Field left(std::string_view str, size_t len) {
		return Field(std::format("LEFT('{}',{})", str, len));
	}
	inline Field right(std::string_view str, size_t len) {
		return Field(std::format("RIGHT('{}',{})", str, len));
	}
	inline Field mid(std::string_view str,size_t pos, size_t len) {
		return Field(std::format("MID('{}',{},{})", str, pos, len));
	}
	inline Field ascii(char ch) {
		return Field(std::format("ASCII('{}')",ch));
	}
	inline Field char_(uint8_t ch) {
		return Field(std::format("CHAR({})",ch));
	}
	inline Field rpad(std::string_view str, size_t len, std::string_view pad) {
		return Field(std::format("RPAD('{}',{},'{}')",str,len,pad));
	}
	inline Field lpad(std::string_view str, size_t len, std::string_view pad) {
		return Field(std::format("LPAD('{}',{},'{}')",str,len,pad));
	}
	inline Field ltrim(std::string_view str) {
		return Field(std::format("LTRIM('{}')",str));
	}
	inline Field rtrim(std::string_view str) {
		return Field(std::format("RTRIM('{}')",str));
	}
	inline Field trim(std::string_view str) {
		return Field(std::format("TRIM('{}')",str));
	}
	inline Field instr(std::string_view str,std::string_view substr) {
		return Field(std::format("INSTR('{}','{}')", str, substr));
	}
	inline Field locate(std::string_view str,std::string_view substr) {
		return Field(std::format("LOCATE('{}','{}')", str, substr));
	}
	inline Field position(std::string_view str,std::string_view substr) {
		return Field(std::format("POSITION('{}' IN '{}')", substr, str));
	}
	inline Field repeat(std::string_view str,size_t count) {
		return Field(std::format("REPEAT('{}',{})", str, count));
	}
	inline Field space(size_t count) {
		return Field(std::format("SPACE({})", count));
	}
	inline Field strcmp(std::string_view str1,std::string_view str2) {
		return Field(std::format("STRCMP('{}','{}')", str1, str2));
	}
	template <typename ...Args, std::enable_if_t<(sizeof...(Args) > 1) && std::conjunction_v<zc::type::traits::is_string<std::decay_t<Args>>...>, int> = 0>
	inline Field elt(size_t n, Args&&...args) {
		std::vector<std::string> vec = { std::format("'{}'", std::forward<Args>(args))... };
		return Field(std::format("ELT({},{})", n, join(vec)));
	}
	template <typename ...Args, std::enable_if_t<(sizeof...(Args) > 1) && std::conjunction_v<zc::type::traits::is_string<std::decay_t<Args>>...>, int> = 0>
	inline Field field(std::string_view str, Args&&...args) {
		std::vector<std::string> vec = { std::format("'{}'", std::forward<Args>(args))... };
		return Field(std::format("FIELD({},{})", str, join(vec)));
	}
	inline Field find_in_set(std::string_view str, std::string_view strlist) {
		return Field(std::format("FIND_IN_SET('{}','{}')", str, strlist));
	}
	inline Field reverse(std::string_view str) {
		return Field(std::format("REVERSE('{}')", str));
	}
	inline Field nullif(std::string_view expr1,std::string_view expr2) {
		return Field(std::format("NULLIF('{}','{}')", expr1, expr2));
	}

	/***************************************时间和日期函数**********************************************/
	inline Field curdate() {
		return Field("CURDATE()");
	}
	inline Field current_date() {
		return Field("CURRENT_DATE()");
	}
	inline Field curtime() {
		return Field("CURTIME()");
	}
	inline Field current_time() {
		return Field("CURRENT_TIME()");
	}
	inline Field now() {
		return Field("NOW()");
	}
	inline Field current_timestamp() {
		return Field("CURRENT_TIMESTAMP()");
	}
	inline Field localtime() {
		return Field("LOCALTIME()");
	}
	inline Field sysdate() {
		return Field("SYSDATE()");
	}
	inline Field localtimestamp() {
		return Field("LOCALTIMESTAMP()");
	}
	inline Field utc_date() {
		return Field("UTC_DATE()");
	}
	inline Field utc_time() {
		return Field("UTC_TIME()");
	}
	//日期与时间戳转换
	inline Field unix_timestamp() {
		return Field("UNIX_TIMESTAMP()");
	}
	inline Field unix_timestamp(std::string_view date) {
		return Field(std::format("UNIX_TIMESTAMP('{}')", date));
	}
	inline Field from_unixtime(std::string_view ts) {
		return Field(std::format("FROM_UNIXTIME('{}')", ts));
	}
	inline Field from_unixtime(unsigned long long ts) {
		return Field(std::format("FROM_UNIXTIME({})", ts));
	}

	//从指定的日期时间中获取年月日
	inline Field date(std::string_view dt) {
		return Field(std::format("DATE('{}')", dt));
	}
	inline Field year(std::string_view dt) {
		return Field(std::format("YEAR('{}')", dt));
	}
	inline Field month(std::string_view dt) {
		return Field(std::format("MONTH('{}')", dt));
	}
	inline Field day(std::string_view dt) {
		return Field(std::format("DAY('{}')", dt));
	}
	inline Field hour(std::string_view dt) {
		return Field(std::format("HOUR('{}')", dt));
	}
	inline Field minute(std::string_view dt) {
		return Field(std::format("MINUTE('{}')", dt));
	}
	inline Field second(std::string_view dt) {
		return Field(std::format("SECOND('{}')", dt));
	}


}
