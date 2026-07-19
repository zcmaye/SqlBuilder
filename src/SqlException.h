#pragma once

#include <stdexcept>

namespace zc::sqlbuilder {
    class SqlException : public std::runtime_error
    {
    public:
        explicit SqlException(const char* modName, const std::string& _Message)
            : runtime_error(_Message.c_str())
            , m_module_name(modName)
        {
        }

        explicit SqlException(const char* modName, const char* _Message)
            : runtime_error(_Message)
            , m_module_name(modName)
        {
        }

        explicit SqlException(const char* _Message)
            : runtime_error(_Message)
            , m_module_name("SQL")
        {
        }


        const char* module_name()const { return m_module_name; }
    private:
        const char* m_module_name{};
    };
}

#define SQL_ASSERT(MODNAME,expr,...) if (!(expr)) throw SqlException(MODNAME,#expr##" : "## __VA_ARGS__)

#define TABLE_ASSERT(expr,...) SQL_ASSERT("TABLE",expr,__VA_ARGS__)
#define FIELD_ASSERT(expr,...) SQL_ASSERT("FIELD",expr,__VA_ARGS__)
#define ZC_ASSERT(expr,...)    SQL_ASSERT("ZC",expr,__VA_ARGS__)

