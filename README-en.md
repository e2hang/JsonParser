Here is the comprehensive English version for your `README-en.md`. It maintains the technical depth of your project while ensuring the terminology aligns with international C++ development standards.

---

# E2hangJson

A high-performance, easy-to-use JSON parsing and serialization library built with **C++20**.

## 🚀 Features

* **Modern C++ Design**: Leverages C++20 features such as `std::source_location` for advanced debugging.
* **Intuitive API**: Supports Python-like dictionary and list operations for seamless data manipulation.
* **High Performance**: Efficient stream-based parsing and optimized string serialization.
* **Industrial-Grade Robustness**: Built-in detailed error reporting including line, column, and byte offset.

---

## 💻 Quick Start

### Example Code

Create a `test.cpp` to demonstrate the basic usage:

```cpp
#include <iostream>
#include "JsonParser.h"
#include "JsonSerializer.h"
#include "JsonValue.h"

int main() {
    // 1. Construct a JSON object manually
    JsonValue root(JsonValue::Type::Map);
    root["name"] = "E2hang";
    root["version"] = 1.0;
    root["features"] = JsonValue::Type::List;
    root["features"].push_back("Fast");
    root["features"].push_back("Modern");

    // 2. Serialize to string
    // Second parameter is indentation (4 spaces)
    std::string jsonStr = JsonSerializer::serialize(root, 4); 
    std::cout << "Serialized JSON:\n" << jsonStr << std::endl;

    // 3. Parse JSON string
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

## 📂 Project Structure

```text
JsonParse/
├── include/             # Header files (.h)
│   ├── JsonValue.h      # Core value type definitions
│   ├── JsonParser.h     # Parsing logic
│   ├── JsonSerializer.h # Serialization logic
│   └── JsonError.h      # Exception & Error handling
├── src/                 # Source files (.cpp)
├── test/                # Unit tests & examples
├── E2hangJson.dll       # Runtime library (Windows)
└── E2hangJson.lib       # MSVC Import library

```

---

## 🛠 API Reference

### 1. Construction & Assignment

Initialize `JsonValue` objects as easily as native variables:

```cpp
// Scalar types
JsonValue name("E2hang");
JsonValue age(25);
JsonValue isDeveloper(true);
JsonValue data(nullptr); // JSON null

// Explicit container initialization
JsonValue obj(JsonValue::Type::Map);
JsonValue arr(JsonValue::Type::List);

```

### 2. Object & Array Operations

Overloaded `[]` operators support intuitive chained access.

**Object Access:**

```cpp
JsonValue user;
user["id"] = 1001;
user["profile"]["bio"] = "Keep coding.";

// Check if a key exists/is a map
if (user.is_map()) { /* ... */ }

```

**Array Access:**

```cpp
JsonValue list(JsonValue::Type::List);
list.push_back(1);
list.push_back("Second");

std::cout << list[1].asString(); // Output: Second

```

### 3. Type-Safe Data Extraction

Use `is_...` checks followed by `as...` getters for safe access:

```cpp
if (val.is_string()) {
    std::string s = val.asString();
}

// Convert to Standard Containers
auto mapData = val.asMap();   // returns std::map<std::string, JsonValue>
auto vecData = val.asList();  // returns std::vector<JsonValue>

```

### 4. Parsing & Serialization

```cpp
// Parsing
JsonParser parser(raw_string);
JsonValue root = parser.parse();

// Serialization: 4 for Pretty Print, -1 for Compact
std::string json = JsonSerializer::serialize(root, 4);

```

---

## 🛠 Advanced Techniques

### 1. Unicode & Escape Handling

* **Escape Support**: Handles `\"`, `\\`, `\/`, `\b`, `\f`, `\n`, `\r`, `\t`.
* **Unicode**: `parse_hex_4()` processes `\uXXXX` sequences into UTF-8 encoded strings.
* **Auto-Escape**: `JsonSerializer` automatically escapes special characters during output.

### 2. Architecture Design

1. **Data Layer (JsonValue)**: Uses a variant-like internal storage for efficient memory management.
2. **Parsing Layer (JsonParser)**: Implements **Recursive Descent Parsing**.
3. **Representation Layer (JsonSerializer)**: Recursive tree traversal with indentation depth management.

---

## ⚙️ Compilation & Installation

### Requirements

* **Compiler**: MSVC (VS 2022+) or GCC (MinGW-w64)
* **Standard**: C++20

### Building the DLL (MSVC)

```bash
cl /I./include /O2 /std:c++20 /utf-8 /EHsc /DJSON_LIBRARY_EXPORT src/*.cpp /LD /Fe:E2hangJson.dll

```

### Compiling Test Program

```bash
cl ./test/test.cpp /I./include /std:c++20 /utf-8 /EHsc E2hangJson.lib /Fe:test_app.exe

```

---

## ⚠️ Troubleshooting

### 1. Linker Error `__imp_...` (Undefined Reference)

* **Reason**: Toolchain mismatch (e.g., mixing MinGW and MSVC).
* **Solution**: Ensure both the DLL and the Application use the same compiler. Do **not** define `JSON_LIBRARY_EXPORT` in your application.

### 2. Encoding Issues (C2001)

* **Reason**: MSVC defaults to local codepage (GBK) for UTF-8 files.
* **Solution**: Always use the `/utf-8` flag during compilation.

---

## 📄 License

This project is licensed under the [MIT License](https://www.google.com/search?q=LICENSE).