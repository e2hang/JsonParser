#define _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#ifndef JSON_VALUE_H_
#define JSON_VALUE_H_

#include <cstddef>
#include <variant>
#include <map>
#include <vector>
#include <string>

#include "JsonConfig.h"

class JsonValue;

class JSON_API JsonValue{
private:
    std::variant<
        std::nullptr_t,
        bool,
        double,
        std::string,
        std::vector<JsonValue>,  //list
        std::map<std::string, JsonValue>  //map
        > v;

    
public:
    enum class Type {
        Null,
        Bool,
        Double,
        String,
        List,
        Map
    };
    //构造函数
    JsonValue();
    JsonValue(Type type);
    JsonValue(const int& n);
    JsonValue(const char* s);    
    JsonValue(const std::nullptr_t& p);
    JsonValue(const bool& p);
    JsonValue(const double& p);
    JsonValue(const std::vector<JsonValue>& p);
    JsonValue(const std::map<std::string, JsonValue>& p);
    JsonValue(const std::string& p);

    //检测类型
    bool is_nullptr() const;
    bool is_bool() const;
    bool is_string() const;
    bool is_list() const;
    bool is_map() const;
    bool is_double() const;

    // is_nullptr();
    bool asBool() const;
    const std::string& asString() const;
    //is_list();
    //bool is_map();
    double asDouble() const;
    std::vector<JsonValue> asList() const;
    std::map<std::string, JsonValue> asMap() const;

    //修改vector
    JsonValue& operator[] (int idx);
    //只读访问vector 重载operator[]
    const JsonValue& operator[] (int idx) const;
    //增加类push_back功能
    void push_back(const JsonValue& val);
    
    //修改map 重载operator[]
    JsonValue& operator[] (const std::string& key);
    //只读访问map
    const JsonValue& operator[] (const std::string& key) const;
    
    void print() const;
};



#endif
