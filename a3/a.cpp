#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <unistd.h>
#include <fstream>

int main() {
    int i = 0;
    while (i < 100) {
        // read a line of input until EOL and store in a string
        std::cout << "a: " << "Hey " << i <<std::endl;
    }
}