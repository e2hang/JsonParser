#include "JsonParse.h"
#include "JsonError.h"
#include "JsonValue.h"
#include <cstddef>

class JsonValue;

//std::string src;
//size_t pos;
//JsonParser::JsonParser(std::string src){  

bool isnum(char c){
    if(c >= '0' && c <= '9') return true;
    return false;
}

JsonValue JsonParser::parse(){
    skip_white_space();
    JsonValue result = parse_value();

    skip_white_space();

    if(peek() != '\0'){
        error(JsonErrorCode::UnexpectedToken,
              "Can't find the end of input"
            );
    }

    return result;
}

void JsonParser::error(JsonErrorCode e, const std::string& msg){
    throw JsonError(e, msg, pos, line, col);
}

void JsonParser::skip_white_space(){
    while(pos < src.size()){
        char c = src[pos];
        if(c == ' ' || c == '\n' || c == '\t' || c == '\r') consume();
        else break;
    }
}
    
char JsonParser::peek() {
    if(pos < src.size()) return src[pos];
    return '\0';
}

char JsonParser::consume() {
    if(pos >= src.size())return '\0';
    char c = src[pos++];
    if(c == '\n'){
        line++;
        col = 1;
    } else {
        col++;
    }
    return c;
}

static std::string encode_utf8(unsigned int code_point) {
    std::string out;
    if (code_point <= 0x7F) {
        // 0xxxxxxx (ASCII)
        out += static_cast<char>(code_point);
    } else if (code_point <= 0x7FF) {
        // 110xxxxx 10xxxxxx
        out += static_cast<char>(0xC0 | ((code_point >> 6) & 0x1F));
        out += static_cast<char>(0x80 | (code_point & 0x3F));
    } else if (code_point <= 0xFFFF) {
        // 1110xxxx 10xxxxxx 10xxxxxx
        out += static_cast<char>(0xE0 | ((code_point >> 12) & 0x0F));
        out += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (code_point & 0x3F));
    } else if (code_point <= 0x10FFFF) {
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (辅助平面，如 Emoji)
        out += static_cast<char>(0xF0 | ((code_point >> 18) & 0x07));
        out += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (code_point & 0x3F));
    }
    return out;
}

// 辅助函数：解析4位十六进制
unsigned int JsonParser::parse_hex_4() {
    unsigned int code = 0;
    for (int i = 0; i < 4; i++) {
        char c = consume();
        code <<= 4;
        if (c >= '0' && c <= '9') code |= (c - '0');
        else if (c >= 'a' && c <= 'f') code |= (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') code |= (c - 'A' + 10);
        else error(JsonErrorCode::InvalidEscape, "Invalid hex digit");
    }
    return code;
}

JsonValue JsonParser::parse_value() {
    // *类内可以直接用 类的函数
    skip_white_space();
    char c = peek();

    // 尾部\0结尾报错
    if (c == '\0') error(JsonErrorCode::UnexpectedEnd, "Unexpected end of input");
    //正常递归解析
    if (c == '\"') return parse_string();
    if (c == '[')  return parse_list();
    if (c == '{')  return parse_map();
    if (c == 't' || c == 'f') return parse_bool();
    if (c == 'n')  return parse_null();
    if (c == '-' || isnum(c)) return parse_number();

    //格式化其他错误并输出
    std::string msg = "Unexpected Character: \'";
    msg += c;
    msg += "\'";
    error(JsonErrorCode::UnexpectedToken, msg);

    return JsonValue();         //  理论上跑不到这里，上面已经throw error了
}

JsonValue JsonParser::parse_null(){
    std::string pattern = "null";
    for(char expected : pattern){
        if(peek() == expected) consume();
        else {
            std::string msg = "Expected 'null', but found unexpected character: '";
            msg += peek();
            msg += "'";
            error(JsonErrorCode::InvalidValue, msg);
        }
    }

    return JsonValue(nullptr);
}

JsonValue JsonParser::parse_bool() {
    //string_view { const char* ptr, size_t len} 不拷贝字符串
    if (peek() == 't') {
        for (char expected : std::string_view("true")) {
            if (peek() == expected) consume();
            else error(JsonErrorCode::InvalidValue, "Invalid boolean: expected 'true'");
        }
        return JsonValue(true);
    } 
    else if (peek() == 'f') {
        for (char expected : std::string_view("false")) {
            if (peek() == expected) consume();
            else error(JsonErrorCode::InvalidValue, "Invalid boolean: expected 'false'");
        }
        return JsonValue(false);
    }
    
    error(JsonErrorCode::InvalidValue, "Expected 'true' or 'false'");
    return JsonValue();

    /*
      非公共前缀可以，公共前缀不行
      解决方法：
      1、增加预读深度，
      2、贪婪匹配+回滚（匹配A，不行则回滚再匹配B）
      3、提取公共前缀，直接匹配后一个字符
      *  json中关键字没有重叠
    */
}

std::string JsonParser::parse_string_raw(){
    consume(); // 消费开始的 "
    std::string content;

    while (peek() != '\"') {
        char c = consume(); // 取得当前字符

        if (c == '\\') {
            // 遇到转义，看下一个字符
            char next = consume(); 
            switch (next) {
                case 'n':  content += '\n'; break;
                case 't':  content += '\t'; break;
                case 'r':  content += '\r'; break;
                case '\\': content += '\\'; break;
                case '\"': content += '\"'; break;
                case 'u': {
                    // TODO: Unicode 处理
                    // 在 parse_string_raw 的 switch (next) 中：
                    unsigned int code = parse_hex_4();
    
                    // 检查是否是 UTF-16 高位代理 (Surrogate Pair)
                    if (code >= 0xD800 && code <= 0xDBFF) {
                        if (consume() != '\\' || consume() != 'u') {
                            error(JsonErrorCode::InvalidEscape, "Expected low surrogate pair \\uXXXX");
                        }
                        unsigned int low = parse_hex_4();
                        if (low < 0xDC00 || low > 0xDFFF) {
                            error(JsonErrorCode::InvalidEscape, "Invalid low surrogate");
                        }
                        // 计算真实的码点: L = (H - 0xD800) * 0x400 + (L - 0xDC00) + 0x10000
                        code = 0x10000 + (code - 0xD800) * 0x400 + (low - 0xDC00);
                    }
    
                    content += encode_utf8(code);
                    break;
                }
                    break;
                case '\0':
                    error(JsonErrorCode::UnexpectedEnd, "Unexpected EOF in escape sequence");
                    break;
                default:
                    error(JsonErrorCode::InvalidEscape, "Invalid escape character");
                    break;
            }
        } 
        else if (c == '\0') {
            // 还没遇到引号就结束了
            error(JsonErrorCode::UnexpectedEnd, "String not closed before end of file");
        } 
        else {
            // 普通字符
            content += c;
        }
    }

    consume(); // 消费结尾的 "
    return content;
}

JsonValue JsonParser::parse_string() {
    return JsonValue(parse_string_raw());
}

JsonValue JsonParser::parse_list(){
    consume();
    skip_white_space();

    std::vector<JsonValue> list;

    if(peek() == ']') {
        consume();
        return JsonValue(std::move(list));
    }
    
    while(true){
        skip_white_space();
        list.push_back(parse_value());
        skip_white_space();
        
        char c = peek();
        if(c == ',') {
            consume();
        } else if (c == ']') {
            consume();
            break; //正常结束循环
        }
        else {
            std::string msg = "Expected \',\' or \']\' but found \'";
            msg += c;
            msg += "\'";
            error(JsonErrorCode::UnexpectedToken, msg);
        }
    }

    return JsonValue(std::move(list));
}

JsonValue JsonParser::parse_map(){
    consume(); // {
    skip_white_space();
    std::map<std::string, JsonValue> mp;

    if(peek() == '}') {
        consume();
        return JsonValue(std::move(mp));
    }

    while(true){
        skip_white_space();
        
        char c = peek();
        if(c != '\"'){
            // 首字符非" 则报错
            std::string msg = "Expect \'\"\', but find \'";
            msg += c;
            msg += "\'";
            error(JsonErrorCode::UnexpectedToken, msg);
        }
        
        std::string key = parse_string_raw();
        // 处理 : 
        skip_white_space();
        if(peek() != ':'){
            std::string msg = "Expected \':\' after object key";
            error(JsonErrorCode::UnexpectedToken, msg);
        }
        consume(); // :
        skip_white_space();
        // 处理 val
        JsonValue val = parse_value();

        // 检查重复 key
        if(mp.find(key) != mp.end()){
            error(JsonErrorCode::DuplicateKey, "Duplicate key found: " + key);
        }
        mp[std::move(key)] = std::move(val);
        //mp.insert({key, val});
        skip_white_space();

        //处理结束后
        char cc = peek();
        if(cc == '}'){
            consume();
            break;
        } else if(cc == ','){
            consume();
            //mp中还有，继续下一个循环
        } else {
            std::string msg = "Expected \',\' or \'}\' in object";
            error(JsonErrorCode::UnexpectedToken, msg);
        }
        
    }

    return JsonValue(std::move(mp));
}

JsonValue JsonParser::parse_number(){
    size_t start = pos;

    //符号
    if(peek() == '-'){
        consume();
        if(!isnum(peek())){
            error(JsonErrorCode::InvalidNumber, "Expected digit after \'-\'");
        }
    }

    //整数
    if(peek() == '0'){
        //前导0: 后面不能直接跟数字
        consume();
        if(isnum(peek())){
            error(JsonErrorCode::InvalidNumber, "Leading 0 is not allowed");
        }
    } else if(isnum(peek())){
        while(isnum(peek())) consume();
    } else {
        //非法字符
        error(JsonErrorCode::InvalidNumber, "Expected digit in a integer part");
    }
    
    //小数
    if(peek() == '.'){
        consume();
        if(!isnum(peek())){
            error(JsonErrorCode::InvalidNumber, "Expected digit after \'.\'");
        }
        while(isnum(peek())){
            consume();
        }
    }

    //指数 e/E
    if(peek() == 'e' || peek() == 'E'){
        consume();
        if(peek() == '+' || peek() == '-'){
            consume();
        }
        if(!isnum(peek())){
            error(JsonErrorCode::InvalidNumber, "Expected digit in exp");
        }
        while(isnum(peek())){
            consume();
        }
    }
    
    //扫描 + 字符串转换为数字 stod
    std::string num_str = src.substr(start, pos - start); // 把str丢给stod

    try {
        double val = std::stod(num_str);
        return JsonValue(val);
    } catch (...) {
        error(JsonErrorCode::InvalidNumber, "Failed to parse number");
    }

    return JsonValue();
}

