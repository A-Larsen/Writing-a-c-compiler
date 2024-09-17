#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <regex>

void ltrim (std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch){
        return !std::isspace(ch);
    }));
}

void match_token(std::string type, std::string str,
                 std::regex reg) {
    std::sregex_iterator current_match (str.begin(), str.end(), reg);
    std::sregex_iterator last_match;

    while(current_match != last_match) {
        std::smatch match = *current_match;
        std::cout << type << ": " << match.str() << "\n";
        current_match++;
    }
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
    uint64_t line_number = 0;

    if (!file.is_open()) {
        std::cout << "could not open file" << std::endl;
        return 1;
    }

    while (std::getline(file, line)) {
        std::regex reg_leading_white_space ("^\\s+");
        if (std::regex_search(line, reg_leading_white_space)) {
            ltrim(line);
        }
        std::cout << line_number << std::endl;

        // find matching token
        std::regex reg_token_identifier ("[a-zA-Z_]\\w*\\b");
        match_token("identifier", line, reg_token_identifier);

        std::regex reg_token_constant ("[0-9]+\\b");
        match_token("constant", line, reg_token_constant);

        std::regex reg_token_int ("int\\b");
        match_token("int", line, reg_token_int);

        ++line_number;
        // remove token from the start of the input
    }

    file.close();

    return 0;
}
