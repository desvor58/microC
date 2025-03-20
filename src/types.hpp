#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "args.hpp"

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#define lastel(X) X[X.size()-1]

std::map<std::string, std::string> types = {
    {"0",  "void"},
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

struct analis_info
{
    std::string file_name;
    u16 line;
    u16 pos;
};

#define cmd_txt_col(r, g, b) "\033[38;2;" << r << ";" << g << ";" << b << "m"
#define cmd_txt_col_reset "\033[00m"

Args targs;

void err(std::string msg, analis_info errc_info, analis_info ref_info, std::string &text)
{
    int ptr;
    u16 cl;

    std::cout
        << errc_info.file_name << ":"
        << errc_info.line << ":"
        << errc_info.pos << ": "
        << cmd_txt_col(255, 0, 0) << "Error: " << cmd_txt_col_reset
        << msg
        << std::endl;

    std::cout << cmd_txt_col(0, 127, 127) << "error calling:" << cmd_txt_col(150, 0, 0) << std::endl;
    std::cout << errc_info.line << " |        ";
    ptr = 0;
    cl = 1;
    while (cl != errc_info.line) {
        if (text[ptr++] == '\n') cl++;
    }
    while (text[ptr] != '\n') {
        if (ptr >= text.size()) break;;
        std::cout << text[ptr++];
    }
    std::cout << std::endl << "           ";

    for (int i = 0; i < errc_info.pos; i++) {
        std::cout << " ";
    }
    std::cout << "" << cmd_txt_col_reset << std::endl;

    if (targs.noref) {
        exit(1);
    }
    // reference
    std::cout
        << cmd_txt_col(0, 127, 127)
        << "reference ("
        << ref_info.file_name << ":"
        << ref_info.line << ":"
        << ref_info.pos << ":"
        << "):"
        << cmd_txt_col(150, 150, 0)
        << std::endl;
    std::cout << ref_info.line << " |        ";

    ptr = 0;
    cl = 1;
    while (cl != ref_info.line) {
        if (text[ptr++] == '\n') cl++;
    }

    while (text[ptr] != '\n') {
        if (ptr >= text.size()) break;;
        std::cout << text[ptr++];
    }

    std::cout << std::endl << "           ";

    for (int i = 0; i < ref_info.pos; i++) {
        std::cout << " ";
    }
    std::cout << "" << cmd_txt_col_reset << std::endl;

    exit(1);
}
