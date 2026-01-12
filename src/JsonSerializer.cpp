#include "JsonSerializer.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>

std::string JsonSerializer::serialize(const JsonValue& value, int indent){
    std::string result;
    serializeValue(value, result, indent, 0);
    return result;
}
    
//直接用JsonValue里面的内容，indent表示缩进个数
void JsonSerializer::serializeValue(const JsonValue& value, std::string& out, int indent, int currIndent) {
    // 处理基础类型 (Null, Bool, Double, String)
    if (const_cast<JsonValue&>(value).is_nullptr()) {
        out += "null";
    } else if (const_cast<JsonValue&>(value).is_bool()) {
        out += (value.asBool() ? "true" : "false");
    } else if (const_cast<JsonValue&>(value).is_double()) {
        std::string s = std::to_string(value.asDouble());
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
        out += s;
    } else if (const_cast<JsonValue&>(value).is_string()) {
        out += escapeString(value.asString());
    } 
    
    // 处理 List (Array)
    else if (const_cast<JsonValue&>(value).is_list()) {
        out += "[";
        const auto& list = value.asList();
        for (size_t i = 0; i < list.size(); ++i) {
            if (indent > 0) {
                out += "\n";
                addIndent(out, currIndent + indent);
            }
            serializeValue(list[i], out, indent, currIndent + indent);
            if (i < list.size() - 1) out += ",";
        }
        if (indent > 0 && !list.empty()) {
            out += "\n";
            addIndent(out, currIndent);
        }
        out += "]";
    } 
    
    // 处理 Map (Object)
    else if (const_cast<JsonValue&>(value).is_map()) {
        out += "{";
        const auto& mp = value.asMap();
        for (auto it = mp.begin(); it != mp.end(); ++it) {
            if (indent > 0) {
                out += "\n";
                addIndent(out, currIndent + indent);
            }
            
            // 写入 Key (必须转义并加引号)
            out += escapeString(it->first);
            out += (indent > 0) ? ": " : ":";
            
            // 递归写入 Value
            serializeValue(it->second, out, indent, currIndent + indent);
            
            if (std::next(it) != mp.end()) out += ",";
        }
        if (indent > 0 && !mp.empty()) {
            out += "\n";
            addIndent(out, currIndent);
        }
        out += "}";
    }
    // 重点：所有的逗号都由父节点负责添加(List 和 Map)
}

void JsonSerializer::addIndent(std::string& out, int indent){
    out.append(indent, ' ');
}

//处理转译字符
std::string JsonSerializer::escapeString(const std::string& s){
    std::string out = "\"";
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b";  break;
            case '\f': out += "\\f";  break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    std::ostringstream oss;
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                    out += oss.str();
                } else {
                    out += c;
                }
        }
    }
    out += "\"";
    return out;
}
