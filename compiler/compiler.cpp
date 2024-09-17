#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <regex>

void ltrim (std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch){
        return !std::isspace(ch);
    }));
}

int main(int argc, char **argv) {

    for (int i = 0; i < argc; ++i) {
        std::string str = argv[i];
        std::regex reg_option_indicator ("^--.*");
        if (std::regex_search(str, reg_option_indicator)) {
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
        std::regex reg_leading_white_space ("^\\s+");
        if (std::regex_search(line, reg_leading_white_space)) {
            ltrim(line);
        }
        // find matching token
        std::regex reg_token_identifier ("[a-zA-Z_]\\w*\\b");
        std::sregex_iterator current_match (line.begin(), line.end(),
                                           reg_token_identifier);

        std::sregex_iterator last_match;

        while(current_match != last_match) {
            std::smatch match = *current_match;
            std::cout << match.str() << "\n";
            current_match++;
        }
        // remove token from the start of the input
    }
    file.close();

    return 0;
}
