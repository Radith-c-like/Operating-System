#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line;
    if (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        int num;
        bool first = true;
        while (iss >> num) {
            if (!first) std::cout << " ";
            std::cout << (num * 7);
            first = false;
        }
        std::cout << std::endl;
    }
    return 0;
}