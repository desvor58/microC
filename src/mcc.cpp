#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <vector>
#include <map>

#include "args.hpp"
#include "types.hpp"

Args args;
std::string ftext = "";
int         pointer = 0;
std::string res_text = "";

struct macro_info
{
    std::string                         name;
    std::pair<std::string, std::string> val;
};

std::vector<std::pair<macro_info, analis_info>>  curr_macroses  {};
std::vector<std::string>                         custom_types   {};

u16 line  = 1;
u16 chpos = 0;

void macroses_exec();

#define uncond_write(wh)                 \
    while (ftext[pointer] != wh) {       \
        res_text += ftext[pointer++];    \
    }                                    \

#define std_anls_inf {args.infile_name, line, chpos}

int main(int argc, char ** argv)
{
    args.create(argc, argv);
    targs = args;

    std::ifstream file(args.infile_name);
    if (file.is_open()) {
        char c;
        while (file.get(c)) {
            ftext += c;
        }
        file.close();
    }
    else {
        std::cerr << "Error: input file not be open (" << args.infile_name << ")" << std::endl;
        exit(1);
    }

    std::string full_digit;

    while (pointer < ftext.size()) {
        chpos++;
        if (ftext[pointer] == '\n') {
            line++;
            chpos = 0;
        }
        if (ftext[pointer] == '/') {
            while (ftext[++pointer] != '/');
            pointer++;
            continue;
        }
        if (ftext[pointer] == '\\') {
            pointer++;
            uncond_write('\\')
            pointer++;
            continue;
        }
        if (ftext[pointer] == '~') {
            pointer++;
            macroses_exec();
            continue;
        }
        if (ftext[pointer] == '!') {
            if (!curr_macroses.empty()) {
                res_text += lastel(curr_macroses).first.val.second;
                if (lastel(curr_macroses).first.name == "s") {
                    res_text += lastel(curr_macroses).first.val.first;
                    res_text += ";\n";
                }
                curr_macroses.pop_back();
            }
        }
        if (ftext[pointer] == '{' || ftext[pointer] == '}') {
            res_text += ftext[pointer];
            res_text += '\n';
        }
        if (ftext[pointer] == '#') {
            res_text += "#define ";
            pointer++;
            uncond_write(' ')
            res_text += ' ';
            pointer++;
            continue;
        }

        if (isalpha(ftext[pointer])) {
    alpha_parse:
            while (isalpha(ftext[pointer]) || ftext[pointer] == '_') {
                res_text += ftext[pointer++];
            }
            if (ftext[pointer] == '\'') {
                res_text += ' ';
                full_digit = "";
                pointer++;
                goto var_decl;
            }
            if (ftext[pointer] == '[') {
                uncond_write(']')
                res_text += ']';
                pointer++;
            }
            if(isdigit(ftext[pointer])) {
                res_text += " = ";
            }
            if (ftext[pointer] == '|') {
                res_text += " = ";
                pointer++;
            }

            uncond_write(';')

            res_text += ";\n";
            continue;
        }
        if ((ftext[pointer] == '*' && isalpha(ftext[++pointer]))) {
            res_text += '*';
            goto alpha_parse;
        }
        if (isdigit(ftext[pointer])) {
            full_digit = "";
            for (;isdigit(ftext[pointer]); pointer++) {
                full_digit += ftext[pointer];
            }

            if (types.count(full_digit) > 0) {
       var_decl:
                bool multi_var_decl_flg = false;
 multi_var_decl:
                int pointers = 0;
                bool isarray  = false;

 check_pointer: if (ftext[pointer] == '*') {
                    pointers++;
                    pointer++;
                }
                if (ftext[pointer] == '*') goto check_pointer;

                if (isalpha(ftext[pointer])) {
                    if (!multi_var_decl_flg) {
                        res_text += types[full_digit];
                    }

                    for (int i = 0; i < pointers; i++) {
                        res_text += '*';
                    }
                    res_text += ' ';
                    while (isalpha(ftext[pointer]) || ftext[pointer] == '_') {
                        res_text += ftext[pointer];
                        pointer++;
                    }
                    // function decl
                    if (ftext[pointer] == '(') {
                        res_text += ftext[pointer];
                        pointer++;
                        std::string buf = "";
                        while (ftext[pointer] != ')') {
                            buf += ftext[pointer];
                            if (types.count(buf) > 0) {
                                res_text += types[buf];
                                res_text += ' ';

                                pointer++;
                                while (ftext[pointer] != ',') {
                                    if (ftext[pointer] == ')') {
                                        res_text += ')';
                                        goto args_parse_end;
                                    }
                                    res_text += ftext[pointer];
                                    pointer++;
                                }
                                res_text += ',';
                                buf = "";
                            }
                            pointer++;
                        }
                        args_parse_end:
                        continue;
                    }

        multi_arrd: if (ftext[pointer] == '[') {
                        uncond_write(']')
                        res_text += ftext[pointer++];
                        isarray = true;
                    }
                    if (ftext[pointer] == '[') goto multi_arrd;

                    if (ftext[pointer] == ',') {
                        multi_var_decl_flg = true;
                        pointer++;
                        res_text += ",";
                        goto multi_var_decl;
                    }

                    if (ftext[pointer] == ';') {
                        res_text += ";\n";
                        continue;
                    }

                    // variable decl
                    res_text += " = ";
                    if (isarray) res_text += "{ ";

                    // variable with separator
                    if (ftext[pointer] == '|') {
                        pointer++;
                    }

                    uncond_write(';')

                    if (isarray) res_text += " }";
                    res_text += ";\n";
                    continue;
                }
            }
        }
        pointer++;
    }
    if (!curr_macroses.empty()) {
        err(
            "Expected '!' for close macross ~" + lastel(curr_macroses).first.name,
            {args.infile_name, line, chpos},
            lastel(curr_macroses).second,
            ftext
        );
    }

    std::ofstream ofile(args.Cfile_name);
    if (ofile.is_open()) {
        ofile.write(res_text.c_str(), res_text.size());
        ofile.close();
    }
    if (args.Cstop) {
        exit(0);
    }

    std::cout << (args.Ccomp +  " " + args.Cfile_name + " -o " + args.outfile_name + " " + args.CCargs).c_str() << std::endl;
    system((args.Ccomp +  " " + args.Cfile_name + " -o " + args.outfile_name + " " + args.CCargs).c_str());

    remove(args.Cfile_name.c_str());
    return 0;
}

void macroses_exec()
{
    // main function
    if (ftext[pointer] == 'm') {
        curr_macroses.push_back(std::pair<macro_info, analis_info>(
            {"m", macroses["m"]},
            std_anls_inf
        ));
        res_text += macroses["m"].first;
    } else
    // include
    if (ftext[pointer] == 'I') {
        std::string arg = "";
        pointer++;
        while (ftext[pointer] != '!') {
            arg += ftext[pointer];
            pointer++;
        }
        res_text += macroses["I"].first;
        res_text += arg;
        res_text += macroses["I"].second;
    } else
    // return
    if (ftext[pointer] == 'r') {
        res_text += macroses["r"].first;

        pointer++;
        uncond_write('!')

        res_text += macroses["r"].second;
    } else
    if (ftext[pointer] == 'n') {
        res_text += macroses["n"].first;

        pointer++;
        uncond_write('!')

        res_text += macroses["r"].second;
    } else
    // printf_s
    if (ftext[pointer] == 'p') {
        res_text += macroses["p"].first;
        pointer++;

        if (ftext[pointer] == '"') {
            res_text += '"';
            pointer++;
            uncond_write('"');
            res_text += '"';
            pointer++;
        }
        uncond_write('!')

        res_text += macroses["p"].second;
    } else
    // while loop
    // "while ("  "){"  "}"
    if (ftext[pointer] == 'w') {
        res_text += "while (";

        // expr write
        pointer++;
        uncond_write(':');
        res_text += ") {\n";

        curr_macroses.push_back(std::pair<macro_info, analis_info>(
            {"w", std::pair<std::string, std::string>("", "}\n")},
            std_anls_inf
        ));
    } else
    // switch
    // "switch ("  ") {\n"  "}\n"
    if (ftext[pointer] == 'S') {
        res_text += "switch (";
        pointer++;
        uncond_write(':')
        res_text += ") {\n";
        curr_macroses.push_back(std::pair<macro_info, analis_info>(
            {"S", std::pair<std::string, std::string>("", "}\n")},
            std_anls_inf
        ));
    } else
    // case
    if (ftext[pointer] == 'c') {
        res_text += "case ";
        pointer++;
        uncond_write(':');
        res_text += ":\n";
    } else
    if (ftext[pointer] == 'b') {
        res_text += "break;\n";
    } else
    // if statement
    if (ftext[pointer] == 'i') {
        res_text += "if (";
        pointer++;
        uncond_write(':');
        res_text += ") {\n";
        curr_macroses.push_back(std::pair<macro_info, analis_info>(
            {"i", std::pair<std::string, std::string>("", "}\n")},
            std_anls_inf
        ));
    } else
    // structs
    if (ftext[pointer] == 's') {
        std::string typedeffed_name = "";
        std::string tname = "";
        bool copy_tname_typedeffed_name = false;

        pointer++;
        if (ftext[pointer] == '(') {
            res_text += "typedef ";
            pointer++;
            while (ftext[pointer] != ')') {
                typedeffed_name += ftext[pointer++];
            }
            if (typedeffed_name == "") {
                copy_tname_typedeffed_name = true;
            }
            pointer++;
        }
        res_text += "struct ";
        while (ftext[pointer] != ':') {
            res_text += ftext[pointer];
            tname += ftext[pointer++];
        }
        if (copy_tname_typedeffed_name) {
            typedeffed_name = tname;
        }
        res_text += "\n{\n";
        curr_macroses.push_back(std::pair<macro_info, analis_info>(
            {"s", std::pair<std::string, std::string>(typedeffed_name, "}")},
            std_anls_inf
        ));
        custom_types.push_back(tname);
    } else
    if (ftext[pointer] == '@') {
        std::string name = "";
        pointer++;
        while (ftext[pointer] != '!') {
            name += ftext[pointer++];
        }
        std::ifstream file(name);
        int offset = 0;
        char c;
        while (file.get(c)) {
            ftext.insert(ftext.begin() + pointer + offset++, c);
        }
        return;
    }
    pointer++;
}