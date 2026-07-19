# SqlBuilder

> 一个简单、灵活且类型安全的 C++23 SQL 构建器库，旨在简化数据库查询操作、提升代码可读性并防范 SQL 注入。

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-orange.svg)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](#平台支持)

## 目录

- [简介](#简介)
- [特性](#特性)
- [平台支持](#平台支持)
- [快速开始](#快速开始)
- [编译安装](#编译安装)
- [集成到项目](#集成到项目)
- [使用示例](#使用示例)
  - [SELECT 查询](#select-查询)
  - [INSERT 插入](#insert-插入)
  - [UPDATE 更新](#update-更新)
  - [DELETE 删除](#delete-删除)
  - [WHERE 条件](#where-条件)
  - [子查询](#子查询)
  - [JOIN 连接](#join-连接)
  - [聚合与函数](#聚合与函数)
  - [可选类型支持](#可选类型支持)
  - [表结构声明](#表结构声明)
- [API 参考](#api-参考)
- [配置选项](#配置选项)
- [项目结构](#项目结构)
- [测试](#测试)
- [常见问题](#常见问题)
- [贡献指南](#贡献指南)
- [许可证](#许可证)

---

## 简介

**SqlBuilder** 是一个使用 C++23 构建的 SQL 语句生成库，提供链式调用的现代 API，让开发者以类型安全的方式构造 SQL 语句，避免手写字符串拼接带来的错误与安全风险。

设计目标：

- **类型安全** — 通过运算符重载与模板约束在编译期捕获类型错误。
- **可读性** — API 镜像 SQL 语法结构，代码即文档。
- **安全** — 自动转义字符串值，防范 SQL 注入。
- **零开销** — 运行时无虚函数开销，构建过程产出 `std::string`。
- **可扩展** — 通过模板支持自定义类型（如 `std::optional`、`oatpp::Object`）。

支持的数据库语法：MySQL、PostgreSQL、SQLite。

## 特性

- 支持 `SELECT / INSERT / UPDATE / DELETE` 全套 DML 语句
- 链式 API 调用，接近原生 SQL 写法
- 字段、表名、条件的运算符重载（`==`、`>`、`<`、`between_and`、`like`、`in`、`is_null` 等）
- 多表 JOIN：`INNER JOIN`、`LEFT/RIGHT/FULL JOIN`、`USING`、`ON`
- 子查询（出现在 SELECT / FROM / WHERE / IN 等位置）
- 聚合函数：`COUNT`、`MAX`、`MIN`、`AVG`、`SUM`
- 常用 SQL 函数库：字符串、日期、`CASE WHEN` 等
- `GROUP BY`、`HAVING`、`ORDER BY`、`LIMIT`
- SQL 注入防护（自动转义）
- 可选支持 `std::optional` 与 `oatpp::Object`
- 字面量运算符：`"ename"_f`、`"emp"_t`
- 表结构宏声明（`DECLARE_TABLE`）

## 平台支持

| 操作系统 | 编译器 | 状态 |
|---------|--------|------|
| Windows | MSVC 2022 (v143) | ✅ |
| Linux   | GCC 13+ / Clang 16+ | ✅ |
| macOS   | Apple Clang 15+ / Clang 16+ | ✅ |

> 要求支持 C++23 的编译器（`std::format`、`std::optional` 等）。

## 快速开始

最小示例：

```cpp
#include "SqlBuilder.h"
#include "Function.h"
#include <print>

using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

int main() {
    auto sql = Select("empno"_f, "ename"_f)
        .from("emp")
        .where("empno"_f == 7788 && "sal"_f > 5000)
        .order_by("sal"_f.desc())
        .limit(10)
        .to_string();

    std::println("{}", sql);
    // 输出: SELECT empno, ename FROM emp WHERE (empno = 7788 AND sal > 5000) ORDER BY sal DESC LIMIT 10
    return 0;
}
```

## 编译安装

### 从源码编译

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 安装到本地

```bash
cmake --install build --prefix "./install"
```

安装目录结构：

```
install/
├── include/        # 头文件
└── lib/            # 静态库 / 动态库 + CMake Config 文件
```

### 构建选项

| 选项 | 默认值 | 说明 |
|---|---|---|
| `BUILD_TESTS` | `ON` | 构建测试可执行文件 |
| `BUILD_SHARED_LIBS` | `OFF` | 构建为共享库（默认静态库） |
| `SQLBUILDER_USE_OPTIONAL` | `ON` | 启用 `std::optional` 支持 |
| `SQLBUILDER_USE_OATPP` | `OFF` | 启用 `oatpp::Object` 支持 |

示例：

```bash
cmake -B build -DBUILD_TESTS=ON -DBUILD_SHARED_LIBS=ON
cmake --build build --config Release
```

安装后会自动生成 CMake Config 文件，支持 `find_package(SqlBuilder)`：

```
install/
├── include/                                              # 头文件
├── lib/
│   ├── SqlBuilder.lib                                    # 静态库 / 动态库
│   └── cmake/SqlBuilder/
│       ├── SqlBuilderConfig.cmake                        # Config 文件
│       ├── SqlBuilderConfigVersion.cmake                 # 版本兼容性文件
│       └── SqlBuilderTargets.cmake                       # 导出 targets
```

## 集成到项目

### 方式一：CMake `find_package`（推荐）

先安装 SqlBuilder 到本地：

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build build --config Release
cmake --install build --prefix "C:/local/SqlBuilder"
```

在使用方项目的 `CMakeLists.txt` 中：

```cmake
cmake_minimum_required(VERSION 3.15)
project(my_app)

# 查找 SqlBuilder（自动继承 PUBLIC 编译选项，如 SQLBUILDER_USE_OPTIONAL）
find_package(SqlBuilder 1.0 REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE SqlBuilder::SqlBuilder)
```

配置时通过 `CMAKE_PREFIX_PATH` 指定安装路径：

```bash
# Windows
cmake -B build -DCMAKE_PREFIX_PATH="C:/local/SqlBuilder" ..

# Linux/macOS
cmake -B build -DCMAKE_PREFIX_PATH="/usr/local/SqlBuilder" ..
```

**特性自动继承**：由于 `SQLBUILDER_USE_OPTIONAL` 等编译选项在 [CMakeLists.txt](CMakeLists.txt) 中声明为 `PUBLIC`，使用方链接 `SqlBuilder::SqlBuilder` 后会自动继承这些宏定义，无需在 CMake 中重复设置。

### 方式二：`add_subdirectory`

```cmake
# 假设 SqlBuilder 位于第三方目录
add_subdirectory(third_party/SqlBuilder)
target_link_libraries(your_target PRIVATE SqlBuilder)
target_include_directories(your_target PRIVATE ${SqlBuilder_SOURCE_DIR}/src)
```

### 方式三：直接包含头文件

将 `src/` 目录加入 include 路径，编译 `Field.cpp`、`Table.cpp`、`StringUtils.cpp` 三个源文件即可。

## 使用示例

### SELECT 查询

```cpp
using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

// 基本查询
auto sql = Select("empno"_f, "ename"_f.as("姓名"), "sal"_f)
    .from("emp")
    .where("sal"_f > 5000)
    .order_by("sal"_f.desc())
    .limit(10, 20)            // LIMIT 20 OFFSET 10
    .to_string();

// 查询所有字段
auto all_sql = Select(all).from("emp").to_string();

// 统计行数
auto count_sql = Select(count_all()).from("emp").to_string();

// 分组聚合
auto group_sql = Select(count("deptno"_f).as("cnt"))
    .from("emp")
    .group_by("deptno"_f, "job"_f)
    .having("cnt"_f > 3)
    .order_by("cnt"_f.desc())
    .limit(10)
    .to_string();
```

### INSERT 插入

```cpp
using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

// 单行插入
auto sql = Insert("empno"_f, "ename"_f, "job"_f)
    .values(7788, "maye", "sales")
    .into("emp")
    .to_string();
// INSERT INTO emp(empno, ename, job) VALUES (7788, 'maye', 'sales')

// 多行插入
auto multi_sql = Insert("empno"_f, "ename"_f, "job"_f)
    .values(7788, "maye", "sales")
           (7789, "rose", "clerk")
           (7790, "jack", "manager")
    .into("emp")
    .to_string();
```

### UPDATE 更新

```cpp
using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

auto sql = Update("emp")
    .set("ename"_f = "maye")("sal"_f = 5200)("comm"_f = nullptr)
    .where("empno"_f == 7788)
    .to_string();
// UPDATE emp SET ename = 'maye', sal = 5200, comm = NULL WHERE empno = 7788

// 字段间运算
auto calc_sql = Update("emp")
    .set("sal"_f = "sal"_f - 5200)
    .where("empno"_f == 7788)
    .to_string();
```

### DELETE 删除

```cpp
using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

auto sql = Delete("emp")
    .where("empno"_f == 7788)
    .to_string();
// DELETE FROM emp WHERE empno = 7788
```

### WHERE 条件

支持逻辑运算符 `&&`、`||`、`!`，以及丰富的比较运算：

```cpp
using namespace zc::sqlbuilder::field_literals;

auto cond = "ename"_f == "maye"
    && "sal"_f > 5000
    && "sal"_f.between_and(3000, 8000)
    && "comm"_f.is_null()
    && "deptno"_f.is_not_null()
    && "hiredate"_f.between_and("2020-01-01", "2020-12-31")
    && "job"_f.like("sal", "%{}%")
    && "empno"_f.in(std::vector{7788, 7789, 7790})
    && "ename"_f.not_in(std::vector<const char*>{"admin", "root"});

auto sql = Select(all).from("emp").where(cond).to_string();
```

### 子查询

```cpp
using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

// WHERE 中的子查询
auto sub = Select("empno"_f).from("emp").where("sal"_f > 5000);
auto sql = Select("empno"_f, "ename"_f)
    .from("emp")
    .where("empno"_f.in(sub))
    .to_string();

// FROM 中的子查询
auto from_sub = Select("empno"_f, "ename"_f, "sal"_f).from("emp").where("sal"_f > 5000);
auto sql2 = Select("empno"_f, "ename"_f)
    .from(from_sub).as("e")
    .to_string();

// SELECT 字段中的子查询
auto select_sub = Select("sal"_f.as("年薪"))
    .from("emp")
    .where("empno"_f == "e.empno"_f);
auto sql3 = Select("empno"_f, Field(select_sub).as("年薪"), "ename"_f)
    .from("emp").as("e")
    .to_string();
```

### JOIN 连接

```cpp
using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

// INNER JOIN + ON
auto sql = Select(all)
    .from("emp")
    .join("dept").on("emp.deptno"_f == "dept.deptno"_f)
    .to_string();

// LEFT JOIN + USING
auto left_sql = Select(all)
    .from("emp")
    .left_join("dept")
    .using_("deptno"_f)
    .to_string();

// 多表 + 别名
auto multi_sql = Select(count(all))
    .from("emp")
    .right_join("dept").on("emp.deptno"_f == "dept.deptno"_f)
    .join("salgrade").as("s")
        .on("emp.sal"_f >= "s.losal"_f && "emp.sal"_f <= "s.hisal"_f)
    .to_string();
```

### 聚合与函数

```cpp
using namespace zc::sqlbuilder;
using namespace zc::sqlbuilder::field_literals;

Select(count(all).as("total"), max("sal"_f).as("max_sal"),
       min("sal"_f).as("min_sal"), avg("sal"_f).as("avg_sal"),
       sum("sal"_f).as("total_sal"));

// 字符串函数
upper("ename"_f);
lower("ename");
substr("ename", 1, 5);
replace("ename", "new_name");

// 日期函数
now();
curdate();
year("hiredate");
date_format("hiredate", "%Y-%m-%d");

// CASE WHEN
auto case_field = case_end(
    "deptno"_f,
    std::vector{10, 20, 30},
    std::vector{"ACCOUNTING", "RESEARCH", "SALES"}
).as("dept_name");
```

### 可选类型支持

启用 `SUPPORT_OPTIONAL` 宏后，可使用 `std::optional` 作为查询条件值，`nullopt` 会自动忽略对应条件：

```cpp
#include "SqlBuilder.h"
using namespace zc::sqlbuilder::field_literals;

std::optional<int> empno = 7788;
std::optional<std::string> ename;
std::optional<int> sal = 6000;

auto sql = Select("empno"_f, "ename"_f)
    .from("emp")
    .where("empno"_f == empno
        && "ename"_f == ename     // ename 为 nullopt，自动跳过
        && "sal"_f > sal)
    .to_string();
// SELECT empno, ename FROM emp WHERE empno = 7788 AND sal > 6000
```

### 表结构声明

通过 `DECLARE_TABLE` 宏快速声明表结构，避免手写字符串字面量：

```cpp
#include "Table.h"
#include "BaseZcMacro.hpp"

DECLARE_TABLE(Emp, emp, empno, ename, job, hiredate, sal, mgr, comm, deptno);
DECLARE_TABLE(Dept, dept, deptno, dname, loc);

void demo() {
    const table::Emp emp;
    const table::Dept dept;

    auto sql = Select(emp.empno, emp.ename, dept.dname)
        .from(emp).join(dept).on(emp.deptno == dept.deptno)
        .where(emp.sal > 1000)
        .order_by(Field(emp.sal).asc())
        .limit(10)
        .to_string();
}
```

## API 参考

### 核心类

| 类 | 头文件 | 说明 |
|---|---|---|
| `Field` | `Field.h` | 字段，支持别名、排序、与值/字段/子查询的运算 |
| `Table` | `Table.h` | 表，支持别名、子查询 |
| `Condition` | `Condition.h` | WHERE / HAVING 条件，支持逻辑组合 |
| `Assign` | `Assign.h` | UPDATE 中的单个 SET 子句 |
| `AssignmentList` | `Assign.h` | 多个 SET 子句集合 |
| `Select` | `SqlBuilder.h` | SELECT 语句构建器 |
| `Insert` | `SqlBuilder.h` | INSERT 语句构建器 |
| `Update` | `SqlBuilder.h` | UPDATE 语句构建器 |
| `Delete` | `SqlBuilder.h` | DELETE 语句构建器 |

### 字面量

| 字面量 | 说明 | 示例 |
|---|---|---|
| `"_f"` / `"_f"` | 创建 `Field` | `"ename"_f` |
| `"_t"` | 创建 `Table` | `"emp"_t` |

### 函数库（`Function.h`）

- 聚合：`count`、`count_all`、`max`、`min`、`avg`、`sum`
- 字符串：`upper`、`lower`、`substr`、`replace`、`length`、`left`、`right`、`trim`、`concat` 等
- 日期：`now`、`curdate`、`curtime`、`year`、`month`、`day`、`unix_timestamp` 等
- 控制：`case_end`、`case_end_for`

### 异常

| 类 | 说明 |
|---|---|
| `SqlException` | 基类，附带模块名 |
| `TableError` | 表相关错误 |
| `FieldError` | 字段相关错误 |

## 配置选项

通过 `src/Config.h` 可启用以下可选特性：

| 宏 | 说明 |
|---|---|
| `SUPPORT_OPTIONAL` | 启用 `std::optional` 支持 |
| `SUPPORT_OATPP` | 启用 `oatpp::Object` 支持（需 oatpp 框架） |

启用前请在 `Config.h` 中取消对应宏的注释，或在 CMake 中通过 `target_compile_definitions` 注入。

## 项目结构

```
SqlBuilder/
├── src/
│   ├── SqlBuilder.h      # Select/Insert/Update/Delete 核心类
│   ├── Field.h/.cpp      # 字段类
│   ├── Table.h/.cpp      # 表类
│   ├── TableDeclare.h    # DECLARE_TABLE 宏定义（快速声明表结构）
│   ├── Condition.h       # 条件类
│   ├── Assign.h          # 赋值/赋值列表
│   ├── Function.h        # SQL 函数封装
│   ├── SqlException.h    # 异常定义
│   ├── Config.h          # 配置宏
│   └── detail/           # 内部实现细节（用户无需直接 include）
│       ├── FormatValue.h     # 值格式化与转义
│       ├── TypeTraits.h      # 类型特性辅助
│       ├── StringUtils.h/.cpp# 字符串工具
│       ├── ZcMacro.hpp       # 宏编程辅助（FOR_EACH）
│       ├── optional_impl.inc # std::optional 支持
│       └── oatpp_impl.inc    # oatpp 支持
├── tests/
│   ├── test_field.cpp        # Field 类测试
│   ├── test_condition.cpp    # Condition 类测试
│   ├── test_assign.cpp       # Assign/AssignmentList 测试
│   ├── test_select.cpp       # Select 语句测试
│   ├── test_insert.cpp       # Insert 语句测试
│   ├── test_update_delete.cpp# Update/Delete 语句测试
│   ├── test_function.cpp     # SQL 函数测试
│   ├── test_table.cpp        # Table 类与表结构声明测试
│   └── test_sql_builder.cpp  # 综合功能测试
├── cmake/
│   └── SqlBuilderConfig.cmake.in  # find_package 模板
├── CMakeLists.txt
├── LICENSE
├── OPTIMIZATION.md       # 优化改进说明
└── README.md
```

## 测试

构建并运行测试：

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build

# Windows
.\build\Debug\test_field.exe
.\build\Debug\test_condition.exe
.\build\Debug\test_assign.exe
.\build\Debug\test_select.exe
.\build\Debug\test_insert.exe
.\build\Debug\test_update_delete.exe
.\build\Debug\test_function.exe
.\build\Debug\test_table.exe

# Linux / macOS
./build/test_field
./build/test_condition
./build/test_assign
./build/test_select
./build/test_insert
./build/test_update_delete
./build/test_function
./build/test_table
```

使用 CTest 批量运行所有测试：

```bash
cd build
ctest -C Debug
```

测试用例覆盖：

| 测试文件 | 覆盖功能 |
|---|---|
| `test_field.cpp` | Field 类（比较运算符、LIKE、IN、BETWEEN、别名、排序、字段运算） |
| `test_condition.cpp` | Condition 类（逻辑运算符、空条件处理） |
| `test_assign.cpp` | Assign/AssignmentList（赋值、NULL 值、链式组合） |
| `test_select.cpp` | Select 语句（JOIN、GROUP BY、HAVING、ORDER BY、LIMIT、子查询） |
| `test_insert.cpp` | Insert 语句（单行/多行插入、字段跳过） |
| `test_update_delete.cpp` | Update/Delete 语句（SET、WHERE、子查询） |
| `test_function.cpp` | SQL 函数（聚合函数、字符串函数、日期函数、CASE WHEN） |
| `test_table.cpp` | Table 类（表名、别名、字面量运算符、operator bool） |
| `test_table_declare.cpp` | 表结构声明（DECLARE_TABLE 宏、字段访问、完整 SQL 构建） |

所有测试使用自定义断言宏，无需外部测试框架。

## 常见问题

### Q1：编译时找不到 `<format>` 头文件？

`SqlBuilder` 依赖 C++23 的 `std::format`，请使用支持 C++23 的编译器：
- MSVC 2022 (v19.34+)
- GCC 13+
- Clang 16+

并在 CMake 中设置 `set(CMAKE_CXX_STANDARD 23)`。

### Q2：`find_package(SqlBuilder)` 找不到配置文件？

请确认：

1. 已执行 `cmake --install` 安装到指定 prefix 路径。
2. 使用方配置时通过 `CMAKE_PREFIX_PATH` 指定了安装路径，例如：
   ```bash
   cmake -B build -DCMAKE_PREFIX_PATH="C:/local/SqlBuilder" ..
   ```
3. 安装目录的 `lib/cmake/SqlBuilder/` 下应包含三个文件：
   - `SqlBuilderConfig.cmake`
   - `SqlBuilderConfigVersion.cmake`
   - `SqlBuilderTargets.cmake`

### Q3：使用 `std::optional` 时条件未自动跳过？

请确保安装时启用了 `SQLBUILDER_USE_OPTIONAL` 选项（默认启用）：

```bash
cmake -B build -DSQLBUILDER_USE_OPTIONAL=ON ...
```

由于该宏通过 `PUBLIC` 编译定义导出，使用方链接 `SqlBuilder::SqlBuilder` 后会自动继承，无需在使用方项目中重复定义。

### Q4：如何避免 SQL 注入？

库内部通过 `escape_string()` 自动转义字符串值。直接传入字符串字面量作为字段名（如 `"ename"_f`）是安全的；用户输入的值通过 `Field::operator==` 等接口传入时会自动加引号并转义。

**注意**：`Field::operator=(const char*)` 会将字符串原样作为 SQL 处理，仅用于受信任的 SQL 字面量，不要传入用户输入。

### Q5：如何生成不同数据库的 SQL（如 PostgreSQL 的 `LIMIT ... OFFSET`）？

当前版本 `LIMIT` 语法为 MySQL/SQLite 风格，方言支持在 [OPTIMIZATION.md](OPTIMIZATION.md) 中已规划。

## 贡献指南

欢迎通过 Issue 或 PR 参与改进：

1. 提交 Issue 描述问题或建议
2. Fork 仓库并创建特性分支
3. 提交 PR，请保持代码风格一致：
   - 命名空间 `zc::sqlbuilder`
   - 头文件使用 `#pragma once`
   - 使用 C++23 特性
   - 新增功能请补充对应测试

## 许可证

本项目基于 [GNU General Public License v3.0](LICENSE) 许可证发布。

