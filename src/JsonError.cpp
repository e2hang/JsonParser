#include "JsonError.h"
#include <sstream>
#include <iomanip>

// 辅助函数：将枚举转为文字
static const char* errorCodeToString(JsonErrorCode code) {
    switch (code) {
        case JsonErrorCode::Ok: return "Ok";
        case JsonErrorCode::UnexpectedToken: return "Unexpected Token";
        default: return "Internal Error";
    }
}

std::string JsonError::to_string() const {
    std::ostringstream oss;
    oss << "[" << errorCodeToString(code_) << "] "
        << message_ 
        << " (at line " << line_ << ", col " << column_ << ")";
    
    // 如果有源码调试信息，也加上
    if (location_.line() != 0) {
        oss << " [Source: " << location_.file_name() << ":" << location_.line() << "]";
    }
    
    return oss.str();
}
