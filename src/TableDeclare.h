#pragma once

#include "detail/ZcMacro.hpp"
#include "Table.h"
#include "Field.h"

#define ZC_MACRO_SINGLE_TO(NAME) \
    Field NAME{[this]{return std::format("{}.{}", table_name, #NAME);}()};

#define ZC_MACRO_PROCESS_SINGLE(pair) \
    ZC_MACRO_SINGLE_TO(pair)

/**
 * 快速定义表格宏.
 * @param CLASS 表格类名（C++中的类名）
 * @param NAME  表格名   (SQL中的表名)
 * @params ...  表格字段名
 * @code DECLARE_TABLE(MyEmp, emp, id, name, age)
 *       SQL_TABLE: emp(id, name, age);
 */
#define DECLARE_TABLE(CLASS, TABLE_NAME, ...) \
    namespace table { \
    struct CLASS { \
        inline static const std::string table_name{#TABLE_NAME}; \
        operator Table() const { return Table(table_name); } \
        ZC_MACRO_FOR_EACH(ZC_MACRO_PROCESS_SINGLE, __VA_ARGS__) \
    };}

/**
 * 以下宏待验证.
 */
#ifdef USE_TABLE_TYPE
#define ZC_MACRO_PAIR_TO(TYPE, NAME) \
    Field NAME{[this]{return std::format("{}.{}", table_name, #NAME);}()}; \
    TYPE _##NAME;

#define ZC_MACRO_PROCESS_PAIR(pair) \
    ZC_MACRO_PAIR_TO pair

#define DECLARE_TABLE_PAIR(CLASS, TBNAME, ...) \
    namespace table { \
    struct CLASS { \
        inline static const char* table_name{#TBNAME}; \
        operator Table() const { return Table(table_name); } \
        ZC_MACRO_FOR_EACH(ZC_MACRO_PROCESS_PAIR, __VA_ARGS__) \
    };}
#endif