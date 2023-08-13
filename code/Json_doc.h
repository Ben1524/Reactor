//
// Created by Stephen Curry on 5/30/2023.
//

#ifndef REACTOR_JSON_DOC_H
#define REACTOR_JSON_DOC_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class JSON {
public:
    JSON() {}
    ~JSON() {}

    // 添加一个字符串键值对
    void addString(const string& key, const string& value) {
        m_data[key] = "\"" + value + "\"";
    }

    // 添加一个整数键值对
    void addInteger(const string& key, int value) {
        m_data[key] = to_string(value);
    }

    // 添加一个浮点数键值对
    void addFloat(const string& key, float value) {
        m_data[key] = to_string(value);
    }

    // 添加一个布尔型键值对
    void addBoolean(const string& key, bool value) {
        m_data[key] = value ? "true" : "false";
    }

    // 添加一个数组键值对
    void addArray(const string& key, const vector<string>& value) {
        string str = "[";
        for (int i = 0; i < value.size(); ++i) {
            str += "\"" + value[i] + "\"";
            if (i != value.size() - 1) {
                str += ",";
            }
        }
        str += "]";
        m_data[key] = str;
    }

    // 打包成JSON字符串
    string pack() const {
        string str = "{";
        for (auto iter = m_data.begin(); iter != m_data.end(); ++iter) {
            str += "\"" + iter->first + "\":" + iter->second;
            if (iter != prev(m_data.end())) {
                str += ",";
            }
        }
        str += "}";
        return str;
    }

private:
    map<string, string> m_data;
};

#endif //REACTOR_JSON_DOC_H
