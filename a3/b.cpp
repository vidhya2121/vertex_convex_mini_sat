#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <unistd.h>
#include <fstream>

int main() {
    while (!std::cin.eof()) {
        // read a line of input until EOL and store in a string
        std::string line;
        std::getline(std::cin, line);
        std::cout << "b: " << line <<std::endl;
    }
}