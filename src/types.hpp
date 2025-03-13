#pragma once

#include <iostream>
#include <vector>
#include <map>

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#define lastel(X) X[X.size()-1]

std::map<std::string, std::string> types = {
    {"7",  "char"},
    {"15", "short"},
    {"31", "int"},
    {"8",  "unsigned char"},
    {"16", "unsigned short"},
    {"32", "unsigned int"},
};

std::map<std::string, std::pair<std::string, std::string>> macroses = {
    {"m", std::pair<std::string, std::string>("int main(int c, char **v)\n{\n", "}\n")},
    {"I", std::pair<std::string, std::string>("#include <", ">\n")},
    {"r", std::pair<std::string, std::string>("return ", ";\n")},
    {"p", std::pair<std::string, std::string>("printf_s(", ");\n")},
    {"n", std::pair<std::string, std::string>("!(", ")")},
};

