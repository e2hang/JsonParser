#define _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#include <iostream>
#include <fstream>
#include <sstream>

#include "E2hangJson.h"

using namespace std;
int main(){
    JsonValue root(JsonValue::Type::Map);
    JsonValue update(JsonValue::Type::List);
    for (int i = 0; i < 10; i++){
        update.push_back(JsonValue(static_cast<double>(i)));
    }

    root["Project"] = "JsonParser";
    root["Version"] = 1.0;
    root["Author"] = "E2hang";
    root["Update"] = update;

    
    //JsonValue features(list);
    JsonSerializer serializer;
    string result = serializer.serialize(root);
    string resultIndent = serializer.serialize(root, 4);
    cout << result << endl;
    cout << resultIndent << endl;

    string path = "output.json";
    ofstream outFile(path);

    if(outFile.is_open()){
        outFile << result;
        outFile.close();
        cout << "Save to File: " << path << endl;
    } else {
        cerr << "Error" << endl;
        return 1;
    }


    string jsonContent;
    ifstream inFile("output.json");
    if (inFile.is_open()) {
        stringstream ss;
        ss << inFile.rdbuf(); // 读取整个文件内容到内存
        jsonContent = ss.str();
        inFile.close();
    } else {
        cerr << "无法读取文件！" << endl;
        return 1;
    }

    // --- 3. 让 JSONPARSER 工作 ---
    try {
        JsonParser parser(jsonContent);
        // 将字符串解析回 JsonValue 对象
        JsonValue root = parser.parse();

        // --- 4. 验证加载是否成功 ---
        cout << "解析成功！正在读取数据..." << endl;
        cout << "Project: " << root["Project"].asString() << endl;
        cout << "Version: " << root["Version"].asDouble() << endl;
        
        // 遍历解析出来的数组
        cout << "Update list: ";
        for (size_t i = 0; i < root["Update"].asList().size(); ++i) {
            cout << root["Update"][i].asDouble() << " ";
        }
        cout << endl;

    } catch (const std::exception& e) {
        cerr << "解析失败: " << e.what() << endl;
    }

    vector<string> errorCases = {
        "{\"a\": 1,}",           // 尾随逗号
        "{name: \"test\"}",      // Key 无引号
        "{\"val\": 0123}",       // 前导零
        "\"Unclosed string",     // 字符串未闭合
        "{\"a\": [1, 2}"         // 数组未闭合
    };

    for (const auto& code : errorCases) {
        JsonParser parser(code);
        try {
            parser.parse();
            cout << "[FAIL] 未能识别错误 JSON: " << code << endl;
        } catch (const JsonError& e) {
            cout << "[OK] 成功捕捉错误: " << e.message() << " -> 原文: " << code << endl;
            cout << "Line = " << e.line() << " Col = " << e.column() << endl;
        }
    }
    return 0;
}
