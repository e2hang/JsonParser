这份 `README.md` 是根据我们解决的 **跨编译器兼容性（MSVC vs MinGW）**、**编码处理**以及**库引用方式**等核心问题量身定制的。它不仅介绍了项目，还包含了避坑指南。

---

# E2hangJson

一个基于 C++20 开发的高性能、易用的 JSON 解析与序列化库。

## 🚀 特性

* **现代 C++ 设计**：充分利用 C++20 特性（如 `std::source_location` 等）。
* **灵活的 API**：支持类似 Python 字典和列表的操作方式。
* **高性能**：支持流式解析和高效的字符串序列化。
* **工业级健壮性**：内置详细的错误报告机制，定位错误发生的行、列及偏移量。

---
明白你的意思了！你希望 README 聚焦于**开发者文档**，重点展示如何调用库里的各种 `JsonValue` 操作、解析和序列化接口。

以下是为你重新编写的、侧重于 **API 使用说明** 的项目文档：

---

## 💻 快速开始

### 示例代码

创建一个 `test.cpp` 来演示如何使用本库：

```cpp
#include <iostream>
#include "JsonParser.h"
#include "JsonSerializer.h"
#include "JsonValue.h"

int main() {
    // 1. 手动构建 JSON 对象
    JsonValue root(JsonValue::Type::Object);
    root["name"] = "E2hang";
    root["version"] = 1.0;
    root["features"] = JsonValue::Type::Array;
    root["features"].push_back("Fast");
    root["features"].push_back("Modern");

    // 2. 序列化为字符串
    std::string jsonStr = JsonSerializer::serialize(root, 4); // 4空格缩进
    std::cout << "Serialized JSON:\n" << jsonStr << std::endl;

    // 3. 解析 JSON 字符串
    JsonParser parser(jsonStr);
    try {
        JsonValue parsedValue = parser.parse();
        std::cout << "Parsed Name: " << parsedValue["name"].asString() << std::endl;
    } catch (const JsonError& e) {
        std::cerr << "Parse Error: " << e.to_string() << std::endl;
    }

    return 0;
}

```

---

# E2hangJson 开发者指南

`E2hangJson` 是一个简洁且功能完备的 C++20 JSON 处理库。它提供了类似于原生数据类型的操作体验，支持对象（Object）、数组（Array）、字符串（String）、数值（Number）、布尔值（Bool）以及空值（Null）。

## 核心类说明

| 类名 | 说明 |
| --- | --- |
| `JsonValue` | **核心容器**。表示一个 JSON 节点，支持递归嵌套。 |
| `JsonParser` | **解析器**。将字符串转换为 `JsonValue` 对象。 |
| `JsonSerializer` | **序列化器**。将 `JsonValue` 对象转换为格式化的字符串。 |
| `JsonError` | **异常类**。提供错误代码及详细的行列定位。 |

---

## 🛠 接口使用说明

### 1. 构建与赋值

你可以通过多种方式初始化 `JsonValue`，操作起来就像使用原生变量：

```cpp
// 基础类型
JsonValue name("E2hang");
JsonValue age(25);
JsonValue isDeveloper(true);
JsonValue data(nullptr); // JSON null

// 容器类型初始化
JsonValue obj(JsonValue::Type::Map);
JsonValue arr(JsonValue::Type::List);

```

### 2. 对象与数组操作

本库重载了 `[]` 运算符，支持链式访问。

**对象操作：**

```cpp
JsonValue user;
user["id"] = 1001;
user["profile"]["bio"] = "Keep coding.";
user["tags"] = JsonValue::Type::Array; 

// 检查是否存在某个键
if (user.is_map()) { /* ... */ }

```

**数组操作：**

```cpp
JsonValue list(JsonValue::Type::Array);
list.push_back(1);
list.push_back("Second");
list.push_back(JsonValue::Type::Object);

// 访问
std::cout << list[1].asString() << std::endl; // 输出: Second

```

### 3. 类型安全的数据提取

为了保证安全，提取数据前可以进行类型检查，随后使用 `as...` 系列函数：

```cpp
if (val.is_string()) {
    std::string s = val.asString();
}

if (val.is_double() || val.is_bool()) {
    double d = val.asDouble();
    bool b = val.asBool();
}

// 转换为标准库容器
auto mapData = val.asMap();   // 返回 std::map<std::string, JsonValue>
auto vecData = val.asList();  // 返回 std::vector<JsonValue>

```

### 4. 解析 JSON 字符串

`JsonParser` 会对输入的字符串进行语法分析：

```cpp
std::string raw = R"({"status": "ok", "code": 200})";
JsonParser parser(raw);

try {
    JsonValue root = parser.parse();
    std::cout << root["status"].asString();
} catch (const JsonError& e) {
    // 发生错误时，e 提供详细信息
    std::cerr << "Error: " << e.message() 
              << " at Line: " << e.line() 
              << ", Col: " << e.column() << std::endl;
}

```

### 5. 序列化 (生成 JSON 文本)

`JsonSerializer` 支持美化输出（带缩进）或紧凑输出：

```cpp
// 参数 2 为缩进空格数。若设为 -1 则生成不带换行的紧凑格式
std::string pretty = JsonSerializer::serialize(root, 4);
std::string compact = JsonSerializer::serialize(root, -1);

```

---

## ⚠️ 开发注意事项

1. **异常处理**：在调用 `asString()`、`asDouble()` 等函数时，如果 `JsonValue` 的实际类型与请求类型不符，库会抛出异常。建议在处理不可信的 JSON 数据时使用 `is_...()` 进行预检。
2. **编码说明**：库内部处理 `std::string`。建议在 Windows 下保持源文件为 **UTF-8** 编码，并在编译时开启 `/utf-8` 标志，以确保中文字符串正常解析。
3. **内存模型**：`JsonValue` 内部使用递归结构管理内存，支持拷贝构造与移动语义，能够高效地在函数间传递。

---

## 🛠 进阶技巧与深度解析

### 1. 细粒度类型检查

除了基础的 `is_string` 等，`JsonValue` 提供了一个 `Type` 枚举，方便在 `switch` 语句中使用，这比连续的 `if-else` 更高效：

```cpp
switch (val.type()) {
    using enum JsonValue::Type;
    case Object: /* 处理对象 */ break;
    case Array:  /* 处理数组 */ break;
    case String: /* 处理字符串 */ break;
    case Number: /* 处理数字 */ break;
    case Bool:   /* 处理布尔 */ break;
    case Null:   /* 处理空值 */ break;
}

```

### 2. 字符串转义与 Unicode 处理 (Internal Logic)

`JsonParser` 内部实现了对 JSON 标准转义字符的支持。如果你需要扩展解析逻辑，可以参考以下机制：

* **转义解析**：支持 `\"`, `\\`, `\/`, `\b`, `\f`, `\n`, `\r`, `\t`。
* **Unicode 支持**：通过 `parse_hex_4()` 接口解析 `\uXXXX` 格式。它会将 4 位十六进制码点转换为 UTF-8 编码存入 `std::string`。
* **序列化转义**：`JsonSerializer::escapeString` 会自动将字符串中的特殊字符（如换行符、引号）转回转义序列，确保生成的 JSON 文件合法。

### 3. 流式调试打印

为了方便快速观察数据结构，`JsonValue` 提供了一个简单的 `print()` 成员函数，直接将内容输出到标准控制台，这在断点调试时非常有用。

```cpp
JsonValue root = parser.parse();
root.print(); // 快速查看控制台输出

```

### 4. 异常定位原理

当解析失败时，`JsonError` 不仅仅是一个错误信息。它通过内部计数器维护了解析进度：

* `offset()`: 距离文件开头的绝对字符位置。
* `line()` / `column()`: 自动计算行号和列号，帮助开发者在大型 JSON 文件中快速定位坏损节点。

---

## 🏗 架构设计

项目遵循**解耦设计**原则，将数据存储、解析逻辑、展现逻辑完全分离：

1. **数据层 (JsonValue)**：采用类似“变体类型”的设计，内部管理一个 `std::variant` 风格的存储结构，负责内存的分配与回收。
2. **解析层 (JsonParser)**：采用 **递归下降解析法 (Recursive Descent Parsing)**。从 `parse_value` 开始，根据首字符探测自动分发到 `parse_object`、`parse_array` 等子模块。
3. **表示层 (JsonSerializer)**：递归遍历树状结构。通过 `addIndent` 维护当前的缩进深度，生成人类可读的格式化文本。

---

## 📝 开发者建议

### 关于 `std::source_location` (C++20)

在 `JsonError` 的构造函数中，我们利用了 C++20 的 `std::source_location`。这意味着当库抛出异常时，它能自动记录是库中哪一行源代码触发了错误，这对于库本身的维护者（也就是你）来说是极大的利好。

### 性能提示

* **移动语义**：在构建大型 JSON 对象时，尽量使用 `std::move`，例如 `root["big_data"] = std::move(another_json_value)`，以避免深拷贝带来的开销。
* **预分配**：如果你知道一个数组的大小，建议先通过标准库方式处理数据，最后再包装进 `JsonValue`。

---

## 🔗 接口一览表 (速查)

| 函数 | 功能 | 备注 |
| --- | --- | --- |
| `push_back(val)` | 向 Array 末尾添加元素 | 仅限 Array 类型 |
| `size()` | 返回对象键数或数组长度 | 通用统计 |
| `to_string()` | 格式化错误报告 | `JsonError` 专属 |
| `serialize(val, indent)` | 静态序列化方法 | `JsonSerializer` 专属 |
| `parse()` | 执行解析并返回 root | `JsonParser` 专属 |

---
## 🛠 编译与安装

### 1. 开发环境要求

* **编译器**：MSVC (Visual Studio 2022+) 或 GCC (MinGW-w64)
* **标准**：C++20

### 2. 编译 DLL 库

项目采用动态链接库 (DLL) 形式导出。编译时需定义 `JSON_LIBRARY_EXPORT` 宏。

**使用 MSVC 命令行：**

```bash
cl /I./include /O2 /std:c++20 /utf-8 /EHsc /DJSON_LIBRARY_EXPORT src/*.cpp /LD /Fe:E2hangJson.dll

```

### 3. 链接与使用

在你的项目中引用 `E2hangJson.lib`（导入库）和头文件。

> **⚠️ 重要提示（避坑指南）：**
> 本库在 Windows 下涉及符号导出。如果你的 DLL 是用 MSVC 编译的，那么测试程序也必须使用 MSVC 编译；混合使用（如用 g++ 链接 MSVC 编译的 DLL）会导致 `undefined reference` 错误。

---

### 编译测试程序 (MSVC)

```bash
cl ./test/test.cpp /I./include /std:c++20 /utf-8 /EHsc E2hangJson.lib /Fe:test_app.exe

```

---

## 📂 项目结构

```text
JsonParse/
├── include/             # 头文件 (.h)
│   ├── JsonValue.h      # 核心数值类型定义
│   ├── JsonParser.h     # 解析逻辑
│   ├── JsonSerializer.h # 序列化逻辑
│   └── JsonError.h      # 异常处理
├── src/                 # 源代码 (.cpp)
├── test/                # 测试用例
├── E2hangJson.dll       # 运行时库
└── E2hangJson.lib       # MSVC 导入库

```

---

## ⚠️ 常见问题排查

### 1. 链接报错 `__imp_...` (Undefined Reference)

* **原因**：编译器不匹配或宏定义缺失。
* **解决**：确保库文件和测试程序使用同一工具链。确保在使用 DLL 时不要定义 `JSON_LIBRARY_EXPORT`。

### 2. 编码问题 `C2001: 常量中有换行符`

* **原因**：MSVC 默认使用 GBK 读取 UTF-8 文件。
* **解决**：编译命令中添加 `/utf-8` 参数。

---

## 📄 开源协议

[MIT License](https://www.google.com/search?q=LICENSE)

---

**你想让我再为你补充一个用于自动化编译的 CMakeLists.txt 模板吗？这样放到 GitHub 上会更专业。**