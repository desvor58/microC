#pragma once

#include <iostream>

class Args
{
public:
    std::string infile_name = "";
    std::string outfile_name = "";
    std::string Cfile_name = "a.c";
    std::string Ccomp = "g++";
    std::string CCargs = "";
    bool        Cstop = false;
    bool        noref = false;

    Args() {}
    void create(int argc, char **argv)
    {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) {
            std::cout << "-i (or first argument)  sourse file .mc" << std::endl;
            std::cout << "-o  output file name (.exe)" << std::endl;
            std::cout << "-cstp  stoppet compilation on C code" << std::endl;
            std::cout << "-CC  set C compiler" << std::endl;
            std::cout << "-CCa  set C compiler arguments ( always last argument!!! )" << std::endl;
            std::cout << "-cfn  set C file name (default: a.c)" << std::endl;
        }
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-i") == 0) {
                infile_name = argv[++i];
                continue;
            }
            if (strcmp(argv[i], "-o") == 0) {
                outfile_name = argv[++i];
                continue;
            }
            if (strcmp(argv[i], "-noref") == 0) {
                noref = true;
                continue;
            }
            if (strcmp(argv[i], "-cfn") == 0) {
                Cfile_name = argv[++i];
                continue;
            }
            if (strcmp(argv[i], "-CC") == 0) {
                Ccomp = argv[++i];
                continue;
            }
            if (strcmp(argv[i], "-CCa") == 0) {
                i++;
                while (i < argc) {
                    CCargs += std::string(argv[i]) + " ";
                    i++;
                }
                continue;
            }
            if (strcmp(argv[i], "-cstp") == 0) {
                Cstop = true;
                continue;
            }
            if (i == 1) {
                infile_name = argv[i];
            }
        }
    }
};