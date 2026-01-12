#define _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#ifndef JSON_ERROR_H_
#define JSON_ERROR_H_

#include <string>
#include <cstddef>   // size_t
#include <source_location>

#include "JsonConfig.h"

enum class JsonErrorCode {
    Ok = 0,

    //语法错误
    UnexpectedToken,
    UnexpectedEnd,
    InvalidNumber,
    InvalidString,
    InvalidEscape,
    InvalidUnicode,

    //结构错误
    MismatchedBracket,
    TrailingComma,
    DuplicateKey,

    //语义错误
    InvalidValue,
    InvalidType,

    //其他
    InternalError               /* 未知内容 */
};

class JSON_API JsonError {
private:
    JsonErrorCode code_;
    std::string message_;

    // 在 JSON 文本中的位置
    size_t offset_;
    size_t line_;
    size_t column_;

    // 产生错误的代码位置
    std::source_location location_;

public:
    JsonError(JsonErrorCode code,
              std::string message,
              size_t offset = 0,
              size_t line = 0,
              size_t column = 0,
              std::source_location loc = std::source_location::current())
        : code_(code),
          message_(std::move(message)),
          offset_(offset),
          line_(line),
          column_(column),
          location_(loc) {}

    //基础访问接口
    JsonErrorCode code() const noexcept { return code_; }
    const std::string& message() const noexcept { return message_; }

    //位置信息
    size_t offset() const noexcept { return offset_; }
    size_t line() const noexcept { return line_; }
    size_t column() const noexcept { return column_; }

    //源码位置信息
    const std::source_location& source_location() const noexcept {
        return location_;
    }

    //简单字符串表示
    std::string to_string() const;


};

#endif
