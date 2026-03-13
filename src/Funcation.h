#pragma once

#include "Column.h"

namespace hdy::tool::sql{

	/**
	 * 返回原始字符串，不使用单引号包裹.
	 */
	inline Column raw(const std::string& str) {
		return Column(str);
	}

		/**
	 * 生成(CASE <value> WHEN <when> THEN <then> ... END)语句.
	 */
	template<typename Value,typename Whens, typename Thens, std::enable_if_t<is_container_v<Whens>&& is_container_v<Thens>, int> = 0>
	inline Column case_end(Value value, const Whens& when, const Thens& then) {
		static_assert(std::is_same_v<Value, typename Whens::value_type>,"value type not equal when type");
		if (when.size() != then.size()) {
			throw std::runtime_error("when and then size not equal");
		}
		std::string result ="(CASE " + format_value(value) + " ";
		for (size_t i = 0; i < when.size(); i++) {
			result += std::format(" WHEN {} THEN {} ", format_value(when[i]), format_value(then[i]));
		}
		return Column(result + "END)");
	}

	template<typename Value,typename ObjectList, typename Fn, std::enable_if_t<is_container_v<ObjectList>, int> = 0>
	inline Column case_end_for(Value value, const ObjectList& objects, Fn&& fn) {
		if (objects.empty()) {
			throw std::runtime_error("objects is empty");
		}
		std::string result = "(CASE " + format_value(value) + " ";
		for (auto& object : objects) { 
			std::pair<std::string, std::string> pair = fn(object);
			result += std::format(" WHEN {} THEN {} ", pair.first, pair.second);
		}
		return Column(result + "END)");
	}

	/***************************************常用函数**********************************************/
	template<typename T, std::enable_if_t<is_string_v<T> || std::is_same_v<T, Column>, int> = 0>
	inline Column upper(const T& v) {
		return Column(std::format("UPPER({})",format_value(v)));
	}
	inline Column ucase(std::string_view v) {
		return upper(v);
	}
	inline Column lower(std::string_view v) {
		return Column(std::format("LOWER('{}')", v));
	}
	inline Column lcase(std::string_view v) {
		return lower(v);
	}
	inline Column replace(std::string_view str,std::string_view newStr) {
		return Column(std::format("REPLACE('{}','{}')", str,newStr));
	}
	inline Column insert(std::string_view str,size_t idx,size_t len,std::string_view newStr) {
		return Column(std::format("INSERT('{}',{},{},'{}')", str, idx, len, newStr));
	}
	inline Column length(std::string_view str) {
		return Column(std::format("LENGTH('{}')", str));
	}
	inline Column char_length(std::string_view str) {
		return Column(std::format("CHAR_LENGTH('{}')", str));
	}
	inline Column substr(std::string_view str, size_t idx, size_t len = -1) {
		if (len == -1)
			return Column(std::format("SUBSTR('{}',{})", str, idx));
		return Column(std::format("SUBSTR('{}',{},{})", str, idx, len));
	}
	inline Column left(std::string_view str, size_t len) {
		return Column(std::format("LEFT('{}',{})", str, len));
	}
	inline Column right(std::string_view str, size_t len) {
		return Column(std::format("RIGHT('{}',{})", str, len));
	}
	inline Column mid(std::string_view str,size_t pos, size_t len) {
		return Column(std::format("MID('{}',{},{})", str, pos, len));
	}
	inline Column ascii(char ch) {
		return Column(std::format("ASCII('{}')",ch));
	}
	inline Column char_(uint8_t ch) {
		return Column(std::format("CHAR({})",ch));
	}
	inline Column rpad(std::string_view str, size_t len, std::string_view pad) {
		return Column(std::format("RPAD('{}',{},'{}')",str,len,pad));
	}
	inline Column lpad(std::string_view str, size_t len, std::string_view pad) {
		return Column(std::format("LPAD('{}',{},'{}')",str,len,pad));
	}
	inline Column ltrim(std::string_view str) {
		return Column(std::format("LTRIM('{}')",str));
	}
	inline Column rtrim(std::string_view str) {
		return Column(std::format("RTRIM('{}')",str));
	}
	inline Column trim(std::string_view str) {
		return Column(std::format("TRIM('{}')",str));
	}
	inline Column instr(std::string_view str,std::string_view substr) {
		return Column(std::format("INSTR('{}','{}')", str, substr));
	}
	inline Column locate(std::string_view str,std::string_view substr) {
		return Column(std::format("LOCATE('{}','{}')", str, substr));
	}
	inline Column position(std::string_view str,std::string_view substr) {
		return Column(std::format("POSITION('{}' IN '{}')", substr, str));
	}
	inline Column repeat(std::string_view str,size_t count) {
		return Column(std::format("REPEAT('{}',{})", str, count));
	}
	inline Column space(size_t count) {
		return Column(std::format("SPACE({})", count));
	}
	inline Column strcmp(std::string_view str1,std::string_view str2) {
		return Column(std::format("STRCMP('{}','{}')", str1, str2));
	}
	template <typename ...Args, std::enable_if_t<(sizeof...(Args) > 1) && std::conjunction_v<is_string<std::decay_t<Args>>...>, int> = 0>
	inline Column elt(size_t n, Args&&...args) {
		std::vector<std::string> vec = { std::format("'{}'", std::forward<Args>(args))... };
		return Column(std::format("ELT({},{})", n, join(vec)));
	}
	template <typename ...Args, std::enable_if_t<(sizeof...(Args) > 1) && std::conjunction_v<is_string<std::decay_t<Args>>...>, int> = 0>
	inline Column field(std::string_view str, Args&&...args) {
		std::vector<std::string> vec = { std::format("'{}'", std::forward<Args>(args))... };
		return Column(std::format("FIELD({},{})", str, join(vec)));
	}
	inline Column find_in_set(std::string_view str, std::string_view strlist) {
		return Column(std::format("FIND_IN_SET('{}','{}')", str, strlist));
	}
	inline Column reverse(std::string_view str) {
		return Column(std::format("REVERSE('{}')", str));
	}
	inline Column nullif(std::string_view expr1,std::string_view expr2) {
		return Column(std::format("NULLIF('{}','{}')", expr1, expr2));
	}

	/***************************************时间和日期函数**********************************************/
	inline Column curdate() {
		return Column("CURDATE()");
	}
	inline Column current_date() {
		return Column("CURRENT_DATE()");
	}
	inline Column curtime() {
		return Column("CURTIME()");
	}
	inline Column current_time() {
		return Column("CURRENT_TIME()");
	}
	inline Column now() {
		return Column("NOW()");
	}
	inline Column current_timestamp() {
		return Column("CURRENT_TIMESTAMP()");
	}
	inline Column localtime() {
		return Column("LOCALTIME()");
	}
	inline Column sysdate() {
		return Column("SYSDATE()");
	}
	inline Column localtimestamp() {
		return Column("LOCALTIMESTAMP()");
	}
	inline Column utc_date() {
		return Column("UTC_DATE()");
	}
	inline Column utc_time() {
		return Column("UTC_TIME()");
	}
	//日期与时间戳转换
	inline Column unix_timestamp() {
		return Column("UNIX_TIMESTAMP()");
	}
	inline Column unix_timestamp(std::string_view date) {
		return Column(std::format("UNIX_TIMESTAMP('{}')", date));
	}
	inline Column from_unixtime(std::string_view ts) {
		return Column(std::format("FROM_UNIXTIME('{}')", ts));
	}
	inline Column from_unixtime(unsigned long long ts) {
		return Column(std::format("FROM_UNIXTIME({})", ts));
	}

	//从指定的日期时间中获取年月日
	inline Column date(std::string_view dt) {
		return Column(std::format("DATE('{}')", dt));
	}
	inline Column year(std::string_view dt) {
		return Column(std::format("YEAR('{}')", dt));
	}
	inline Column month(std::string_view dt) {
		return Column(std::format("MONTH('{}')", dt));
	}
	inline Column day(std::string_view dt) {
		return Column(std::format("DAY('{}')", dt));
	}
	inline Column hour(std::string_view dt) {
		return Column(std::format("HOUR('{}')", dt));
	}
	inline Column minute(std::string_view dt) {
		return Column(std::format("MINUTE('{}')", dt));
	}
	inline Column second(std::string_view dt) {
		return Column(std::format("SECOND('{}')", dt));
	}


}
