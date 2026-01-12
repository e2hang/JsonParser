#include "JsonValue.h"
#include <cstddef>
#include <stdexcept>
#include <variant>
#include <iostream>
#include <iomanip>
#include <vector>

class JsonValue;

//重载visited
template <class... Ts> struct overloaded : Ts... {using Ts::operator()...; };

template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

JsonValue::JsonValue(): v(nullptr) {}
JsonValue::JsonValue(const char* s): v(std::string(s)) {}
JsonValue::JsonValue(const int& n): v(static_cast<double>(n)) {}
JsonValue::JsonValue(const std::nullptr_t& p): v(p) {}
JsonValue::JsonValue(const bool& p): v(p) {}
JsonValue::JsonValue(const std::vector<JsonValue>& p): v(p) {}
JsonValue::JsonValue(const std::map<std::string, JsonValue>& p): v(p) {}
JsonValue::JsonValue(const std::string& p): v(p) {}
JsonValue::JsonValue(const double& p): v(p) {}

JsonValue::JsonValue(Type type){
    switch(type){
        case Type::Null: v = nullptr; break;
        case Type::Bool: v = false; break;
        case Type::Double: v = 0.0; break;
        case Type::String: v = std::string(); break;
        case Type::List: v = std::vector<JsonValue>(); break;
        case Type::Map: v = std::map<std::string, JsonValue>(); break;
    }
}



bool JsonValue::is_nullptr() const{
    if(std::holds_alternative<std::nullptr_t>(v)){
        return true;
    } else {
        return false;
    }
}

bool JsonValue::is_bool() const{
    if(std::holds_alternative<bool>(v)) return true;
    else return false;
}

bool JsonValue::is_double() const{
    if(std::holds_alternative<double>(v)) return true;
    else return false;
}

bool JsonValue::is_string() const{
    if(std::holds_alternative<std::string>(v)) return true;
    else return false;
}

bool JsonValue::is_list() const{
    if(std::holds_alternative<std::vector<JsonValue>>(v)) return true;
    else return false;
}

bool JsonValue::is_map() const{
    if(std::holds_alternative<std::map<std::string, JsonValue>>(v)) return true;
    else return false;
}

const std::string& JsonValue::asString() const {
    if(auto* p = std::get_if<std::string>(&(this->v))) return *p;
    throw std::runtime_error("JsonValue is not a string");
}

bool JsonValue::asBool() const {
    if(auto* p = std::get_if<bool>(&(this->v))) return *p;
    throw std::runtime_error("JsonValue is not a bool type");
}

double JsonValue::asDouble() const {
    if(auto* p = std::get_if<double>(&(this->v))) return *p;
    throw std::runtime_error("JsonValue is not a double");
}

std::vector<JsonValue> JsonValue::asList() const {
    if(auto* p = std::get_if<std::vector<JsonValue>>(&v)) return *p;
    throw std::runtime_error("JsonValue is not a List");
}

std::map<std::string, JsonValue> JsonValue::asMap() const{
    if(auto* p = std::get_if<std::map<std::string, JsonValue>>(&v)) return *p;
    throw std::runtime_error("JsonValue is not a Map");
}

JsonValue& JsonValue::operator[] (int idx) {
    if(!is_list()) v = std::vector<JsonValue>();
    return std::get<std::vector<JsonValue>>(v).at(idx);  
}

const JsonValue& JsonValue::operator[] (int idx) const {
    if(!is_list()) throw std::runtime_error("JsonValue is not a list(vector)");
    return std::get<std::vector<JsonValue>>(v).at(idx);  
}

void JsonValue::push_back(const JsonValue& val){
    if(!std::holds_alternative<std::vector<JsonValue>>(v)){
        throw std::runtime_error("Not a list");
    }
    std::get<std::vector<JsonValue>>(v).push_back(val);
}

JsonValue& JsonValue::operator[] (const std::string& key){
    if(is_nullptr()) v = std::map<std::string, JsonValue>();
    if(!is_map()) throw std::runtime_error("JsonValue is not a map");
    return std::get<std::map<std::string, JsonValue>>(v)[key];
}

const JsonValue& JsonValue::operator[] (const std::string& key) const{
    if (auto p = std::get_if<std::map<std::string, JsonValue>>(&v)){
        auto it = p->find(key);
        if (it != p->end()){
            return it->second;
        }
        throw std::runtime_error("Key is not found" + key);
    }
    throw std::runtime_error("JsonValue is not a object(map)");
}

void JsonValue::print() const{
    //递归打印 vector, map
    /* static auto recursive_print = [](auto& self, const JsonValue& jval, int indent) -> void {
        std::string space(indent, ' '); // 缩进几个
    };
    */
    std::visit(overloaded {
            [&](std::nullptr_t) {std::cout << "null"; },
                [&](bool val) {std::cout << (val ? "true" : "false"); },
                [&](double val) {std::cout << val; },
                [&](const std::string& val) {std::cout << "\"" << val << "\"";},
                [&](const std::vector<JsonValue>& vec) {
                    std::cout << "[";
                    for(size_t i = 0; i < vec.size(); ++i){
                        vec[i].print();
                        if (i < vec.size() - 1) std::cout << ",";
                        //std::cout << "\n";
                    }
                    std::cout << "]";
                },
                [&](const std::map<std::string, JsonValue>& mp) {
                    std::cout << "{" ;
                    for(auto it = mp.begin(); it != mp.end(); ++it){
                        std::cout << "\"" << it->first << "\":";
                        it->second.print();
                        if(std::next(it) != mp.end()) std::cout << ",";
                    }
                    std::cout << "}" ;
                }
                }, v);
}
