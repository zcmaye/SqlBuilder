#pragma once

#include <stdexcept>

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

class TableError : public SqlException {
public:
    explicit TableError(const std::string& _Message)
        : SqlException("Table", _Message.c_str())
    {
    }

    explicit TableError(const char* _Message)
		: SqlException("Table", _Message)
    {
    }
};

class FieldError : public SqlException {
public:
    explicit FieldError(const std::string& _Message)
        : SqlException("Field", _Message.c_str())
    {
    }

    explicit FieldError(const char* _Message)
        : SqlException("Field", _Message)
    {
    }
};