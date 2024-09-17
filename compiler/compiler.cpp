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
    std::ifstream file (file_name);
    std::string line;

    if (!file.is_open()) {
        std::cout << "could not open file" << std::endl;
        return 1;
    }

    while (std::getline(file, line)) {
        std::regex reg ("^\\s+");
        if (std::regex_search(line, reg)) {
            std::cout << line << "\n";
        }
    }
    file.close();

    return 0;
}
