/**
 * @file main.cpp
 * @author Mattia Mascarello (mattia.mascarello@edu.unito.it)
 * @brief PlagiaCode main file, detects difference between code submissions 
 * @version 0.1
 * @date 2022-12-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
static bool verbose = false;
#include "levenshteinDistance.cpp"
#include "comparator.cpp"
#define VERSION "0.1"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WINDOWS
#define OPTIONDASH "/"
#else
#define OPTIONDASH "-"
#endif
static char *programName;

static void version()
{
    std::cerr << "PlagiaCode version" << VERSION << "\n 2022, Mattia Mascarello" << std::endl;
}

static void help()
{
    std::cerr << "\
Use: " << programName
              << " [-i] [FILE] ... \n\
Compares a list of file or directories recursively \n\
with the Levenshtein distance algorithm\n\
to aide the detection of plagiarism\n\
  " << OPTIONDASH
              << "h \t\\thelp (this message)\n\
  " << OPTIONDASH
              << "v \t\t version\n\
  " << OPTIONDASH
              << "i \t\t do not ignore whitespaces\n\
  " << OPTIONDASH
              << "r \t\t do not perform recursive search\n\
  " << OPTIONDASH
              << "ve \t\t enable verbose output\n\
              "
              << std::endl;
}

/**
 * @brief Argument flag with system-default shash
 * 
 * @param arg argument
 * @return std::string argument flag 
 */
std::string argplat(const char *arg)
{
    return std::string(OPTIONDASH) + std::string(arg);
}
int main(int argc, char *argv[])
{

    programName = argv[0];
    switch (argc)
    {
    case 1: // only the program name
        help();
        break;

    default:
        if (argplat("h").compare(argv[1]) == 0)
        {
            help();
        }
        else if (argplat("v").compare(argv[1]) == 0)
        {
            version();
        }
        else
        {

            bool invisibleIgnore = true;
            bool recursive = true;
            int startFrom = 1;
            if (argplat("ve").compare(argv[startFrom]) == 0)
            {
                verbose = true;
                startFrom++;
            }
            if (argplat("i").compare(argv[startFrom]) == 0)
            {
                if (verbose)
                    std::cout << "Not ignoring whitespaces" << std::endl;
                invisibleIgnore = false;
                startFrom++;
            }
            if (argplat("r").compare(argv[startFrom]) == 0)
            {
                if (verbose)
                    std::cout << "Don't perform recursive search" << std::endl;
                recursive = false;
                startFrom++;
            }
            if (startFrom == argc)
            { // there are no input files (we consider also the name of the program at arg[0])
                std::cerr << "E: Specify some input files/directories" << std::endl;
            }
            else
            {
                std::vector<std::string> filePaths;
                for (int i = startFrom; i < argc; i++)
                {
                    filePaths.push_back(std::string(argv[i]));
                }
                std::vector<std::filesystem::directory_entry> files = scanFiles(filePaths, recursive);
                std::map<std::string, long> map = startComparison(files, invisibleIgnore);
                printSorted(map);
            }
            break;
        }
    }
}
