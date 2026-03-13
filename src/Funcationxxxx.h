#pragma once

#include "Column.h"

namespace hdy::tool::sql{

	/***************************************常用函数**********************************************/
	inline RawStr upper(std::string_view v) {
		return RawStr(std::format("UPPER('{}')", v));
	}
	inline RawStr ucase(std::string_view v) {
		return upper(v);
	}
	inline RawStr lower(std::string_view v) {
		return RawStr(std::format("LOWER('{}')", v));
	}
	inline RawStr lcase(std::string_view v) {
		return lower(v);
	}
	inline RawStr replace(std::string_view str,std::string_view newStr) {
		return RawStr(std::format("REPLACE('{}','{}')", str,newStr));
	}
	inline RawStr insert(std::string_view str,size_t idx,size_t len,std::string_view newStr) {
		return RawStr(std::format("INSERT('{}',{},{},'{}')", str, idx, len, newStr));
	}
	inline RawStr length(std::string_view str) {
		return RawStr(std::format("LENGTH('{}')", str));
	}
	inline RawStr char_length(std::string_view str) {
		return RawStr(std::format("CHAR_LENGTH('{}')", str));
	}
	inline RawStr substr(std::string_view str, size_t idx, size_t len = -1) {
		if (len == -1)
			return RawStr(std::format("SUBSTR('{}',{})", str, idx));
		return RawStr(std::format("SUBSTR('{}',{},{})", str, idx, len));
	}
	inline RawStr left(std::string_view str, size_t len) {
		return RawStr(std::format("LEFT('{}',{})", str, len));
	}
	inline RawStr right(std::string_view str, size_t len) {
		return RawStr(std::format("RIGHT('{}',{})", str, len));
	}
	inline RawStr mid(std::string_view str,size_t pos, size_t len) {
		return RawStr(std::format("MID('{}',{},{})", str, pos, len));
	}
	inline RawStr ascii(char ch) {
		return RawStr(std::format("ASCII('{}')",ch));
	}
	inline RawStr char_(uint8_t ch) {
		return RawStr(std::format("CHAR({})",ch));
	}
	inline RawStr rpad(std::string_view str, size_t len, std::string_view pad) {
		return RawStr(std::format("RPAD('{}',{},'{}')",str,len,pad));
	}
	inline RawStr lpad(std::string_view str, size_t len, std::string_view pad) {
		return RawStr(std::format("LPAD('{}',{},'{}')",str,len,pad));
	}
	inline RawStr ltrim(std::string_view str) {
		return RawStr(std::format("LTRIM('{}')",str));
	}
	inline RawStr rtrim(std::string_view str) {
		return RawStr(std::format("RTRIM('{}')",str));
	}
	inline RawStr trim(std::string_view str) {
		return RawStr(std::format("TRIM('{}')",str));
	}
	inline RawStr instr(std::string_view str,std::string_view substr) {
		return RawStr(std::format("INSTR('{}','{}')", str, substr));
	}
	inline RawStr locate(std::string_view str,std::string_view substr) {
		return RawStr(std::format("LOCATE('{}','{}')", str, substr));
	}
	inline RawStr position(std::string_view str,std::string_view substr) {
		return RawStr(std::format("POSITION('{}' IN '{}')", substr, str));
	}
	inline RawStr repeat(std::string_view str,size_t count) {
		return RawStr(std::format("REPEAT('{}',{})", str, count));
	}
	inline RawStr space(size_t count) {
		return RawStr(std::format("SPACE({})", count));
	}
	inline RawStr strcmp(std::string_view str1,std::string_view str2) {
		return RawStr(std::format("STRCMP('{}','{}')", str1, str2));
	}
	template <typename ...Args, std::enable_if_t<(sizeof...(Args) > 1) && std::conjunction_v<is_string<std::decay_t<Args>>...>, int> = 0>
	inline RawStr elt(size_t n, Args&&...args) {
		std::vector<std::string> vec = { std::format("'{}'", std::forward<Args>(args))... };
		return RawStr(std::format("ELT({},{})", n, join(vec)));
	}
	template <typename ...Args, std::enable_if_t<(sizeof...(Args) > 1) && std::conjunction_v<is_string<std::decay_t<Args>>...>, int> = 0>
	inline RawStr field(std::string_view str, Args&&...args) {
		std::vector<std::string> vec = { std::format("'{}'", std::forward<Args>(args))... };
		return RawStr(std::format("FIELD({},{})", str, join(vec)));
	}
	inline RawStr find_in_set(std::string_view str, std::string_view strlist) {
		return RawStr(std::format("FIND_IN_SET('{}','{}')", str, strlist));
	}
	inline RawStr reverse(std::string_view str) {
		return RawStr(std::format("REVERSE('{}')", str));
	}
	inline RawStr nullif(std::string_view expr1,std::string_view expr2) {
		return RawStr(std::format("NULLIF('{}','{}')", expr1, expr2));
	}

	/***************************************时间和日期函数**********************************************/
	inline RawStr curdate() {
		return RawStr("CURDATE()");
	}
	inline RawStr current_date() {
		return RawStr("CURRENT_DATE()");
	}
	inline RawStr curtime() {
		return RawStr("CURTIME()");
	}
	inline RawStr current_time() {
		return RawStr("CURRENT_TIME()");
	}
	inline RawStr now() {
		return RawStr("NOW()");
	}
	inline RawStr current_timestamp() {
		return RawStr("CURRENT_TIMESTAMP()");
	}
	inline RawStr localtime() {
		return RawStr("LOCALTIME()");
	}
	inline RawStr sysdate() {
		return RawStr("SYSDATE()");
	}
	inline RawStr localtimestamp() {
		return RawStr("LOCALTIMESTAMP()");
	}
	inline RawStr utc_date() {
		return RawStr("UTC_DATE()");
	}
	inline RawStr utc_time() {
		return RawStr("UTC_TIME()");
	}
	//日期与时间戳转换
	inline RawStr unix_timestamp() {
		return RawStr("UNIX_TIMESTAMP()");
	}
	inline RawStr unix_timestamp(std::string_view date) {
		return RawStr(std::format("UNIX_TIMESTAMP('{}')", date));
	}
	inline RawStr from_unixtime(std::string_view ts) {
		return RawStr(std::format("FROM_UNIXTIME('{}')", ts));
	}
	inline RawStr from_unixtime(unsigned long long ts) {
		return RawStr(std::format("FROM_UNIXTIME({})", ts));
	}

	//从指定的日期时间中获取年月日
	inline RawStr date(std::string_view dt) {
		return RawStr(std::format("DATE('{}')", dt));
	}
	inline RawStr year(std::string_view dt) {
		return RawStr(std::format("YEAR('{}')", dt));
	}
	inline RawStr month(std::string_view dt) {
		return RawStr(std::format("MONTH('{}')", dt));
	}
	inline RawStr day(std::string_view dt) {
		return RawStr(std::format("DAY('{}')", dt));
	}
	inline RawStr hour(std::string_view dt) {
		return RawStr(std::format("HOUR('{}')", dt));
	}
	inline RawStr minute(std::string_view dt) {
		return RawStr(std::format("MINUTE('{}')", dt));
	}
	inline RawStr second(std::string_view dt) {
		return RawStr(std::format("SECOND('{}')", dt));
	}
}
