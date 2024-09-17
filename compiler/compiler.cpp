#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <regex>

int main(int argc, char **argv) {

    for (int i = 0; i < argc; ++i) {
        std::string str = argv[i];
        std::regex reg ("^--.*");
        if (std::regex_search(str, reg)) {
            std::cout << "regex found at " << i << std::endl;
        }
    }
    std::string file_name = argv[argc - 1];
    std::cout << "filename: \"" << file_name << "\""<< std::endl;
    /* std::ofstream file; */
    /* file.open("") */
    return 0;
}
