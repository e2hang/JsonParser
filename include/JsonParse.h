#define _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#ifndef JSON_PARSE_H_
#define JSON_PARSE_H_

#include "JsonValue.h"
#include "JsonError.h"
#include <string>

#include "JsonConfig.h"

class JsonValue;

class JSON_API JsonParser{
public:
    /* explicit: 必须是显式转换，不能隐式转换 */
    explicit JsonParser(std::string source) : src(std::move(source)), pos(0), line(1), col(1) {}
    JsonValue parse();

private:
    std::string src;
    size_t pos;
    size_t line;
    size_t col;

    void skip_white_space();
    char peek();
    char consume();
    void error(JsonErrorCode e, const std::string& msg);
    
    JsonValue parse_value();
    JsonValue parse_number();
    JsonValue parse_string();
    unsigned int parse_hex_4();
    std::string parse_string_raw();
    JsonValue parse_list();
    JsonValue parse_map();
    JsonValue parse_bool();
    JsonValue parse_null();

};

#endif
