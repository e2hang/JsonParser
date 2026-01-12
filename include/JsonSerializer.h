#ifndef JSON_SERIALIZER_H_
#define JSON_SERIALIZER_H_

#include "JsonValue.h"
#include <string>

#include "JsonConfig.h"

class JSON_API JsonSerializer{
public:
    static std::string serialize(const JsonValue& value, int indent = 0);
    
private:
    static void serializeValue(const JsonValue& value, std::string& out, int indent, int currIndent);
    static void addIndent(std::string& out, int indent);
    static std::string escapeString(const std::string& s);
};

#endif
