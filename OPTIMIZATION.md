# SqlBuilder 项目优化改进说明文档

> 本文档基于对 `SqlBuilder` 项目的全面代码审查，记录已修复的问题以及后续仍可优化的方向。
>
> **状态说明**：
> - ✅ **已修复**：用户已处理的问题（仅供历史记录）
> - 🔧 **待修复**：仍存在并建议处理的问题
>
> 优先级说明：
> - **P0 (严重)**：必须修复的 Bug 或安全漏洞
> - **P1 (重要)**：影响功能正确性、可维护性或跨平台兼容性的问题
> - **P2 (改进)**：提升代码质量、性能和开发体验的优化建议
> - **P3 (增强)**：可选的新功能或进一步增强项

---

## 目录

- [1. 项目概述](#1-项目概述)
- [2. 严重问题 (P0)](#2-严重问题-p0)
- [3. 重要问题 (P1)](#3-重要问题-p1)
- [4. 代码质量改进 (P2)](#4-代码质量改进-p2)
- [5. 构建系统优化 (P2)](#5-构建系统优化-p2)
- [6. 测试体系改进 (P2)](#6-测试体系改进-p2)
- [7. 性能优化 (P2)](#7-性能优化-p2)
- [8. API 设计增强 (P3)](#8-api-设计增强-p3)
- [9. 文档完善 (P3)](#9-文档完善-p3)
- [10. 修改优先级总览](#10-修改优先级总览)

---

## 1. 项目概述

`SqlBuilder` 是一个基于 C++23 的 SQL 构建器库，提供类型安全的链式 API 来构建 `SELECT / INSERT / UPDATE / DELETE` 语句，支持子查询、JOIN、聚合函数等功能，并可选支持 `std::optional` 和 `oatpp::Object`。

**项目结构**：
- `src/` - 核心库源码（头文件 + 少量 cpp）
- `tests/` - 两个示例性测试文件
- `CMakeLists.txt` - 构建脚本
- `README.md` - 项目说明文档
- `LICENSE` - GPL v3 许可证

---

## 2. 严重问题 (P0)

### 2.1 ✅ 已修复：`Assign::operator const std::string()` 悬空引用

**位置**：[src/Assign.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Assign.h#L68) 第 68 行

原 `operator const std::string& ()const` 返回对 `to_string()` 临时对象的引用，属于未定义行为。已修改为返回值：

```cpp
operator const std::string ()const { return to_string(); }  // ✅ 现在返回值
```

### 2.2 🔧 待修复：`AssignmentList::operator const std::string&()` 仍存在悬空引用

**位置**：[src/Assign.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Assign.h#L107) 第 107 行

```cpp
class AssignmentList {
public:
    operator const std::string& ()const { return to_string(); }  // BUG!
};
```

**问题**：`to_string()` 返回一个临时 `std::string` 对象，而 `operator const std::string&` 返回对该临时对象的引用，导致 **悬空引用 (dangling reference)**，调用方访问属于未定义行为。

**修复建议**（与 `Assign` 类保持一致）：
```cpp
operator const std::string ()const { return to_string(); }
```

或保留引用版本但需缓存结果到成员变量（不推荐，增加复杂度）。

### 2.3 ✅ 已修复：`Field::operator=(const char*)` SQL 注入风险

**位置**：[src/Field.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Field.h#L161)

已将隐式的 `operator=(const char* raw_sql)` 改为显式 API `raw_set()`：

```cpp
// 可选：支持字面量字符串（不加引号）
AssignmentList raw_set(const char* raw_sql) const {
    return Assign(_name, raw_sql);  // 信任用户提供的 SQL
}
```

避免了与 `operator=(const std::string&)` 的重载歧义，且语义清晰，调用方明确知道传入的是原始 SQL。

### 2.4 🔧 待修复：`escape_string` 转义不完整

**位置**：[src/FormatValue.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/FormatValue.h#L17-L33)

```cpp
//case '%': escaped += "\\%"; break; // LIKE通配符%转义
//case '_': escaped += "\\_"; break; // LIKE通配符_转义
```

**问题**：
1. LIKE 通配符 `%` 和 `_` 的转义被注释掉。`Field::like()` 中用户输入的 `%`、`_` 未被转义，可能导致 LIKE 注入或意外匹配。
2. 未处理 NUL 字符 (`\0`)，可能在某些数据库驱动中截断字符串。
3. 未处理其他控制字符（如 `\x1a` MySQL 终止符）。

**修复建议**：
- 拆分为两个函数：
  - `escape_string`（通用转义，不加 `%`、`_`）
  - `escape_like_string`（LIKE 模式专用，额外转义 `%` 和 `_`）
- 在 [src/Field.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Field.h) 的 `like()` 中调用 `escape_like_string`。

### 2.5 ✅ 已修复：`Table.cpp` 头文件名大小写错误

**位置**：[src/Table.cpp](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Table.cpp#L2)

原 `#include "SqlBUilder.h"` 已修正为 `#include "SqlBuilder.h"`，跨平台编译通过。

### 2.6 ✅ 已修复：生产代码中使用 `printf`

**位置**：[src/SqlBuilder.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlBuilder.h) `Insert::to_string` 附近

原 `printf("[field_string]: skip column %s\n", ...)` 调试输出已删除，库代码不再污染用户 stdout。

---

## 3. 重要问题 (P1)

### 3.1 ✅ 已修复：`Condition` 的逻辑运算符缺少 `const` 修饰

**位置**：[src/Condition.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Condition.h#L16-L37)

`operator &&`、`operator ||`、`operator !` 已全部添加 `const` 修饰：

```cpp
Condition operator &&(const Condition& other) const { ... }
Condition operator ||(const Condition& other) const { ... }
Condition operator !() const { ... }
```

现在可以对 `const Condition` 调用逻辑运算符。

### 3.2 🔧 待修复：`Insert::skip_columns()` 仅基于首行判断

**位置**：[src/SqlBuilder.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlBuilder.h#L289-L300)

```cpp
std::vector<int> skip_columns()const {
    std::vector<int> skip_columns_;
    auto& value = _values.front();  // 只看第一行
    for (size_t i = 0; i < value.size(); i++) {
        if (value[i] == InvalidValue) {
            skip_columns_.push_back(i);
        }
    }
    return skip_columns_;
}
```

**问题**：多行 INSERT 时，如果其他行的对应位置不是 `InvalidValue`，会被错误地跳过。例如：

```cpp
Insert(...).values(InvalidValue, "name1")  // 第一行第二列被跳过
          (123, InvalidValue);             // 第二行第一列应该是 InvalidValue，但实际被跳过的是第二列
```

**修复方案**（推荐方案 B）：
- 选项 A：每行独立判断跳过哪些列（语义复杂，需要不同结构的 SQL）。
- 选项 B：检测所有行的 `InvalidValue` 列位置必须一致，否则抛异常。
- 选项 C：禁止多行 INSERT 中使用 `InvalidValue`（在 `values()` 时检测）。

### 3.3 ✅ 已修复：`optional_impl.inc` 中 `between_and` 未处理 `nullopt`

**位置**：[src/optional_impl.inc](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/optional_impl.inc#L80-L87)

已与其他 optional 重载保持一致，`nullopt` 时返回空 `Condition`：

```cpp
template<typename T, ...>
Condition between_and(const std::optional<T>& lower, const std::optional<T>& upper) const {
    if (!lower.has_value() || !upper.has_value()) {
        return Condition();  // ✅ 与其他 optional 操作保持一致
    }
    return Condition(_name, "BETWEEN", std::format("{} AND {}", format_value(lower), format_value(upper)));
}
```

### 3.4 ✅ 已修复：`Update::to_string` 中存在未使用变量

**位置**：[src/SqlBuilder.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlBuilder.h#L399)

原多余的 `auto con = _condition.to_string();` 已删除。

### 3.5 ✅ 已修复：`Select::limit` 方言支持

**位置**：[src/SqlBuilder.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlBuilder.h#L110-L116)

已改为通用的 `LIMIT count OFFSET offset` 语法（PostgreSQL/SQLite 兼容，MySQL 8.0+ 也支持）：

```cpp
Select& limit(size_t offset, size_t count) {
    _limit = std::format("LIMIT {} OFFSET {}", count, offset);
    return *this;
}
```

**备注**：MySQL 旧版本（< 8.0）不支持该语法。若需兼容更老版本或 SQL Server/Oracle，仍需引入方言抽象层。

### 3.6 🔧 部分修复：`Table` 默认构造函数允许空表名

**位置**：[src/Table.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Table.h#L14)

`Table` 仍保留默认构造函数 `_name` 默认为空，但 `to_string()` 中已添加断言：

```cpp
std::string to_string() const {
    TABLE_ASSERT(!_name.empty(), "Table name is empty");  // ✅ 运行时检测
    ...
}
```

属于运行时防御，可接受。如果希望编译期约束，可考虑删除默认构造函数（但会影响 `std::vector` 等容器使用）。

---

## 4. 代码质量改进 (P2)

### 4.1 ✅ 已修复：文件命名拼写错误

`src/Funcation.h` 已重命名为 [src/Function.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Function.h)，所有引用已同步更新。

### 4.2 🔧 待优化：类内 `#include` 的 hack 写法

**位置**：[src/Field.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Field.h#L271-L272)

```cpp
private:
    std::string _name;
    ...
#include "optional_impl.inc"
#include "oatpp_impl.inc"
};
```

**问题**：在类定义内部使用 `#include` 来扩展类的成员函数，是一种可读性较差的 hack。虽然避免了 `#ifdef` 在类内部散落，但增加维护成本。

**修复建议**：
- 方案 A：使用 `#ifdef SUPPORT_OPTIONAL` / `#ifdef SUPPORT_OATPP` 直接内联到 `Field` 类中。
- 方案 B：将 optional/oatpp 支持抽取为独立的自由函数（模板），通过 ADL 调用。

### 4.3 🔧 待优化：命名空间不一致

**问题**：
- [src/StringUtils.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/StringUtils.h) 中的 `StringList` 类位于全局命名空间。
- `StringUtils::split` 等函数位于 `StringUtils` 命名空间。
- [src/SqlException.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlException.h) 中的 `SqlException`、`TableError`、`FieldError` 位于全局命名空间。

**修复建议**：统一放置在 `hdy::tool::sql` 命名空间下（或 `hdy::tool::utils`）。

### 4.4 🔧 待优化：保留的 `#if 0` 死代码

**位置**：
- [src/Assign.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Assign.h#L11) 第 11 行 — 旧版 `Assign` 类
- [src/SqlBuilder.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlBuilder.h#L24) 第 24 行 — 旧版字符串重载

**问题**：保留的死代码增加阅读负担，且 Git 历史已能恢复旧版本。

**修复建议**：删除所有 `#if 0` 块。

### 4.5 ✅ 已修复：`Config.h` 中宏定义不清晰

**位置**：[src/Config.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Config.h)

已改为通过 CMake option 控制：

```cpp
#pragma once

#ifdef SQLBUILDER_USE_OPTIONAL
#include <optional>
#define SUPPORT_OPTIONAL
#endif

#ifdef SQLBUILDER_USE_OATPP
#include "common/core/Types.hpp"
#define SUPPORT_OATPP
#endif
```

并在 [CMakeLists.txt](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/CMakeLists.txt#L24-L25) 中提供 `SQLBUILDER_USE_OPTIONAL` / `SQLBUILDER_USE_OATPP` 选项。

### 4.6 ✅ 已修复：`InvalidValue` 改为 `constexpr string_view`

**位置**：[src/FormatValue.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/FormatValue.h#L10)

```cpp
inline constexpr std::string_view InvalidValue { "InvalidValue" };  // ✅ constexpr
```

避免了程序启动时构造全局 `std::string`。

### 4.7 🔧 待优化：缺少 `noexcept` 修饰

**位置**：多个类的访问器

**问题**：许多不修改对象状态的访问器未声明 `noexcept`。

**示例修复**：
```cpp
// Field.h
std::string name() const noexcept { return _name; }
std::optional<std::string> alias() const noexcept { return _alias; }
bool has_alias() const noexcept { return _alias.has_value(); }

// Condition.h
bool empty() const noexcept { return _sql.empty(); }
const std::string& to_string() const noexcept { return _sql; }
```

### 4.8 🔧 待优化：`Insert::value_string` 字符串拼接逻辑过于复杂

**位置**：[src/SqlBuilder.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlBuilder.h#L313-L336)

```cpp
const std::string delimiter = " , ";
...
// 跳过列时拼接位置可能错误
if (str.ends_with(delimiter)) {
    str.resize(str.size() - delimiter.size());
}
```

**问题**：
1. 使用 `" , "`（带空格）作为分隔符，与 `field_string` 中使用的 `", "`（一个空格）不一致。
2. 通过 `ends_with` 删除尾部分隔符的逻辑容易出错。

**修复建议**：与 `field_string` 保持一致，使用 "首元素不加分隔符" 的惯用法：

```cpp
std::string value_string(const std::vector<int>& skips) const {
    std::string result;
    bool first_row = true;
    for (auto& value : _values) {
        if (!first_row) result += ", ";
        result += "(";
        bool first_col = true;
        for (size_t i = 0; i < value.size(); ++i) {
            if (std::binary_search(skips.begin(), skips.end(), i)) continue;
            if (!first_col) result += ", ";
            result += value[i];
            first_col = false;
        }
        result += ")";
        first_row = false;
    }
    return result;
}
```

### 4.9 🔧 待优化：`oatpp_impl.inc` 中命名空间限定缺失

**位置**：[src/oatpp_impl.inc](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/oatpp_impl.inc#L2)

```cpp
template<typename T, std::enable_if_t <is_object_v<T>, int > = 0>  // 缺少 hdy::type::traits::
```

**问题**：依赖 ADL 查找 `is_object_v`，但 `is_object_v` 定义在 `hdy::type::traits` 命名空间。当前能工作是因为 `Field` 类位于 `hdy::tool::sql`，ADL 可能失败。

**修复建议**：使用完整限定名 `hdy::type::traits::is_object_v<T>`，或在使用前 `using namespace hdy::type::traits;`。

### 4.10 🔧 待优化：仓库中包含压缩包

**问题**：项目根目录存在 `SqlBuilder.7z` 压缩包，不应纳入版本控制。

**修复建议**：
- 从仓库删除 `SqlBuilder.7z`。
- 在 [.gitignore](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/.gitignore) 中添加：
  ```
  *.7z
  *.zip
  *.rar
  ```

### 4.11 🔧 待优化：注释风格不统一

**问题**：项目中混用 `//`、`///`、`/*! */`、`/** */` 等多种注释风格，部分注释存在错别字。

**修复建议**：统一采用 Doxygen 风格的 `///` 或 `//!` 注释，并配置 Doxygen 生成 API 文档。

---

## 5. 构建系统优化 (P2)

### 5.1 ✅ 已修复：`CMakeLists.txt` 全面改进

**位置**：[CMakeLists.txt](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/CMakeLists.txt)

已完成以下改进：
- ✅ 添加项目版本号（`VERSION 1.0.0`）和描述信息
- ✅ 移除 `file(GLOB ...)`，改为显式列出源文件
- ✅ 配置 `target_include_directories`，支持 `BUILD_INTERFACE` / `INSTALL_INTERFACE`
- ✅ 添加 CMake option：`BUILD_TESTS`、`SQLBUILDER_USE_OPTIONAL`、`SQLBUILDER_USE_OATPP`、`BUILD_SHARED_LIBS`
- ✅ 设置 C++23 标准、隐藏符号可见性、PIC
- ✅ 启用编译器警告（MSVC `/W4`、GCC/Clang `-Wall -Wextra -Wpedantic -Werror`）
- ✅ 添加 `GNUInstallDirs` 安装规则
- ✅ 通过 `CMakePackageConfigHelpers` 生成 `SqlBuilderConfigVersion.cmake`，支持 `find_package(SqlBuilder)`
- ✅ 通过 CTest 集成测试

### 5.2 ✅ 已修复：完整支持 `find_package(SqlBuilder)`

**位置**：[CMakeLists.txt](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/CMakeLists.txt#L64-L93)、[cmake/SqlBuilderConfig.cmake.in](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/cmake/SqlBuilderConfig.cmake.in)

已完成 `find_package` 所需的四件套：

1. **`write_basic_package_version_file`** — 生成 `SqlBuilderConfigVersion.cmake`，支持版本兼容性检查
2. **`configure_package_config_file`** — 从模板 `cmake/SqlBuilderConfig.cmake.in` 生成 `SqlBuilderConfig.cmake`
3. **`install(EXPORT SqlBuilderTargets NAMESPACE SqlBuilder::)`** — 导出 `SqlBuilderTargets.cmake`，使用方通过 `SqlBuilder::SqlBuilder` 引用
4. **`install(FILES ...)`** — 安装 Config 和 ConfigVersion 文件到 `lib/cmake/SqlBuilder/`

同时修复了头文件安装的遗漏：[src/Field.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Field.h#L271) 通过 `#include "optional_impl.inc"` 和 `#include "oatpp_impl.inc"` 依赖 `.inc` 文件，安装规则中补充了 `PATTERN "*.inc"`。

**使用方集成方式**：

```bash
# 安装 SqlBuilder
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --install build --prefix "C:/local/SqlBuilder"
```

```cmake
# 使用方 CMakeLists.txt
find_package(SqlBuilder 1.0 REQUIRED)
target_link_libraries(my_app PRIVATE SqlBuilder::SqlBuilder)
```

```bash
cmake -B build -DCMAKE_PREFIX_PATH="C:/local/SqlBuilder" ..
```

由于 `SQLBUILDER_USE_OPTIONAL` 等 `target_compile_definitions` 声明为 `PUBLIC`，使用方链接 `SqlBuilder::SqlBuilder` 后会自动继承这些宏定义，无需手动定义。

### 5.3 🔧 待优化：缺少 CI/CD 配置

**建议**：添加 GitHub Actions 配置文件 `.github/workflows/ci.yml`，包含：
- 多平台测试（Windows/Ubuntu/macOS）
- 多编译器测试（MSVC/GCC/Clang）
- ASAN/UBSAN 检测
- 静态分析（clang-tidy/cppcheck）

---

## 6. 测试体系改进 (P2)

### 6.1 🔧 待优化：当前测试的不足

**位置**：[tests/test_sql_builder.cpp](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/tests/test_sql_builder.cpp)、[tests/test_table.cpp](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/tests/test_table.cpp)

**问题**：
1. 没有断言，仅靠 `std::println` 打印结果人工检查。
2. 没有测试框架（如 GoogleTest、Catch2、doctest）。
3. 测试覆盖不全：`StringUtils`、`FormatValue` 中的边界情况未测试。
4. 没有异常路径测试。
5. 没有性能基准。

### 6.2 🔧 改进建议

**引入 Catch2**（轻量级、单头文件）：
```cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "SqlBuilder.h"

TEST_CASE("Select basic", "[select]") {
    using namespace hdy::tool::sql;
    using namespace hdy::tool::sql::literals;
    
    auto sql = Select("empno"_c, "ename"_c)
        .from("emp")
        .where("empno"_c == 1)
        .to_string();
    
    REQUIRE(sql == "SELECT empno, ename FROM emp WHERE empno = 1");
}

TEST_CASE("Select throws on empty columns", "[select]") {
    using namespace hdy::tool::sql;
    REQUIRE_THROWS_AS(Select().to_string(), std::runtime_error);
}
```

**关键测试用例**：
- SQL 注入测试：`std::string("hello' OR '1'='1")` 应被正确转义
- 空字段、空表名、空条件应抛异常
- 多行 INSERT 的字段跳过逻辑
- LIKE 转义
- 子查询嵌套
- `std::optional` 的 nullopt 处理

---

## 7. 性能优化 (P2)

### 7.1 ✅ 已修复：`Select::to_string` 内存分配优化

**位置**：[src/SqlBuilder.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlBuilder.h#L124-L154)

已使用 `result.reserve(256)` 预估容量，并通过 `result += ' '`、`result += ...` 避免 `+` 产生临时字符串：

```cpp
std::string result;
result.reserve(256);	//预估容量
result = std::format("SELECT {}", hdy::tool::sql::join(_columns));
result += ' ';
result += hdy::tool::sql::join(_join_tables, " ");
```

### 7.2 🔧 待优化：`escape_string` 的内存分配

**位置**：[src/FormatValue.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/FormatValue.h#L17-L33)

**当前**：`escaped.reserve(param.size() * 2)` 过度分配。

**优化**：先扫描一遍统计需要转义的字符数量，再 `reserve(param.size() + escape_count)`。

### 7.3 🔧 待优化：使用 `std::string_view` 参数

许多接受 `std::string` 的函数可改为 `std::string_view` 避免不必要的拷贝，例如：
```cpp
explicit Field(std::string_view name) : _name(name) {}
```
（注意：`_name` 是 `std::string`，赋值时仍会拷贝，但调用方传 `const char*` 时避免先转 `std::string`）

### 7.4 🔧 待优化：`to_string()` 返回 `const` 引用 vs 值

**问题**：部分 `to_string()` 返回 `std::string`（值拷贝），部分返回 `const std::string&`（引用），行为不一致。

**建议**：内部已缓存结果（如 `Condition::_sql`）的类返回 `const std::string&`；每次重新生成的（如 `Field::to_string`、`AssignmentList::to_string`）返回 `std::string`。

---

## 8. API 设计增强 (P3)

### 8.1 🔧 缺少的 SQL 功能

| 功能 | 重要性 | 备注 |
|---|---|---|
| `DISTINCT` | 高 | `Select::distinct()` |
| `UNION / INTERSECT / EXCEPT` | 高 | 集合操作 |
| `EXISTS / NOT EXISTS` 子查询 | 高 | 子查询增强 |
| `ON DUPLICATE KEY UPDATE` | 中 | MySQL 特性 |
| `INSERT INTO ... SELECT` | 中 | 批量插入 |
| `NATURAL JOIN` | 低 | 较少使用 |
| `CROSS JOIN` | 低 | 笛卡尔积 |
| `WITH (CTE)` | 高 | 公用表表达式 |
| `WINDOW` 函数 | 中 | OLAP 场景 |
| `CASE WHEN` 表达式（无值版本） | 中 | 已有 `case_end`，但仅支持值匹配 |
| `GROUP_CONCAT / STRING_AGG` | 低 | 字符串聚合 |

### 8.2 🔧 类型安全增强

**当前**：`Field` 仅包含字符串名称，丢失了类型信息。

**建议**：引入模板化字段（可选）：
```cpp
template<typename T>
class TypedField {
    std::string name_;
public:
    TypedField(std::string_view name) : name_(name) {}
    // 类型安全的运算符重载
    Condition operator==(T value) const;
    // ...
};
```

### 8.3 🔧 流式输出

**当前**：需要调用 `to_string()`。

**建议**：支持 `std::ostream`：
```cpp
friend std::ostream& operator<<(std::ostream& os, const Select& s) {
    return os << s.to_string();
}
```

### 8.4 🔧 使用 C++20 Concepts 替代 `std::enable_if_t`

**当前**：
```cpp
template<typename T, std::enable_if_t<hdy::type::traits::is_string_v<T> || ..., int> = 0>
Condition operator==(T value) const;
```

**改进**：定义 concept：
```cpp
template<typename T>
concept SqlValue = hdy::type::traits::is_string_v<T> || std::is_arithmetic_v<T>;

template<typename T>
requires SqlValue<T>
Condition operator==(T value) const;
```

### 8.5 🔧 链式调用的 `&&` 限定

为了支持移动语义优化，链式方法可返回 `Field&&`：
```cpp
Field& as(const std::string& alias) & { ... }
Field as(const std::string& alias) && { 
    Field ret(std::move(*this)); 
    ret._alias = alias; 
    return ret; 
}
```

---

## 9. 文档完善 (P3)

### 9.1 ✅ 已修复：README.md 已完善

**位置**：[README.md](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/README.md)

已重写为标准说明文档，包含：
- 顶部徽章（License、C++23、平台）
- 完整目录导航
- 简介、特性、平台支持
- 快速开始与编译安装
- 三种集成方式
- 10 个完整使用示例（SELECT/INSERT/UPDATE/DELETE/WHERE/子查询/JOIN/聚合/可选类型/表结构声明）
- API 参考表
- 配置选项说明
- 项目结构图
- 测试运行命令
- 5 个常见问题 FAQ
- 贡献指南
- 许可证

### 9.2 🔧 待完善：补充文档

建议新增：
- `docs/api.md` - 详细 API 参考
- `docs/design.md` - 架构设计说明
- `CHANGELOG.md` - 版本变更日志
- `CONTRIBUTING.md` - 贡献指南（独立文件）
- 配置 Doxygen（`Doxyfile`）自动生成 API 文档

---

## 10. 修改优先级总览

### ✅ 已修复项（12 项）

| 优先级 | 问题 | 位置 |
|---|---|---|
| **P0** | `Assign::operator const std::string()` 悬空引用 | [Assign.h:68](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Assign.h#L68) |
| **P0** | `Field::operator=(const char*)` SQL 注入风险 | [Field.h:161](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Field.h#L161) |
| **P0** | `Table.cpp` 头文件大小写错误 | [Table.cpp:2](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Table.cpp#L2) |
| **P0** | 生产代码 `printf` | SqlBuilder.h |
| **P1** | `Condition` 缺少 const 修饰 | [Condition.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Condition.h) |
| **P1** | `between_and` nullopt 处理 | [optional_impl.inc](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/optional_impl.inc) |
| **P1** | `Update::to_string` 未使用变量 | SqlBuilder.h |
| **P1** | `Select::limit` 方言支持 | [SqlBuilder.h:110](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/SqlBuilder.h#L110) |
| **P2** | 文件命名 `Funcation → Function` | [Function.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Function.h) |
| **P2** | `Config.h` CMake option 控制 | [Config.h](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/Config.h) |
| **P2** | `InvalidValue` 改为 constexpr | [FormatValue.h:10](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/src/FormatValue.h#L10) |
| **P2** | CMakeLists.txt 全面改进 | [CMakeLists.txt](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/CMakeLists.txt) |
| **P2** | 完整支持 `find_package(SqlBuilder)` | [CMakeLists.txt](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/CMakeLists.txt#L64-L93) + [cmake/SqlBuilderConfig.cmake.in](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/cmake/SqlBuilderConfig.cmake.in) |
| **P2** | `Select::to_string` 性能优化 | SqlBuilder.h |
| **P3** | README.md 完善 | [README.md](file:///c:/Users/Maye/Downloads/libSrc/SqlBuilder/README.md) |

### 🔧 待修复项（按优先级排序）

| 优先级 | 问题 | 影响 |
|---|---|---|
| **P0** | `AssignmentList::operator const std::string&` 悬空引用 (2.2) | 运行时崩溃/未定义行为 |
| **P0** | `escape_string` 未处理 LIKE 通配符 `%`、`_` (2.4) | 安全漏洞 |
| **P1** | `Insert::skip_columns` 多行逻辑 (3.2) | 数据正确性 |
| **P2** | 类内 `#include` hack (4.2) | 可维护性 |
| **P2** | 命名空间不一致 (4.3) | 代码组织 |
| **P2** | `#if 0` 死代码 (4.4) | 可读性 |
| **P2** | 缺少 noexcept (4.7) | 代码质量 |
| **P2** | `Insert::value_string` 字符串拼接逻辑 (4.8) | 可读性/正确性 |
| **P2** | oatpp 命名空间限定 (4.9) | 可移植性 |
| **P2** | 仓库包含 7z (4.10) | 仓库卫生 |
| **P2** | 注释风格不统一 (4.11) | 可读性 |
| **P2** | 缺少 CI/CD (5.3) | 工程化 |
| **P2** | 测试体系 (6.x) | 质量保证 |
| **P2** | `escape_string` 内存分配 (7.2) | 性能 |
| **P2** | 使用 `string_view` 参数 (7.3) | 性能 |
| **P3** | 缺少 SQL 功能 (8.1) | 功能增强 |
| **P3** | 类型安全增强 (8.2) | 类型安全 |
| **P3** | 流式输出 (8.3) | 易用性 |
| **P3** | Concepts 替代 (8.4) | 现代化 |
| **P3** | 补充文档 (9.2) | 易用性 |

---

## 附：建议的实施顺序

1. **第一阶段（紧急修复）**：完成 P0 中剩余项 — `AssignmentList` 悬空引用 + `escape_string` LIKE 转义。
2. **第二阶段（功能完善）**：完成 P1 中的 `Insert::skip_columns` 多行逻辑。
3. **第三阶段（重构）**：完成 P2 中的代码质量改进、CI/CD 与测试体系（`find_package` 支持已完成）。
4. **第四阶段（增强）**：逐步添加 P3 的新功能和完善文档。

每个阶段完成后，建议运行完整测试套件（含 ASAN/UBSAN）验证无回归。

---

*本文档基于代码审查生成，反映截至 2026-07-18 的项目状态。建议作为后续重构的参考基线。*
