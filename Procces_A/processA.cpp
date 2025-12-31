#include <iostream>
#include <sstream>
#include <string>

const int N = 22;

int main() {
    std::string line;
    if (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        int num;
        bool first = true;
        while (iss >> num) {
            if (!first) std::cout << " ";
            std::cout << (num + N);
            first = false;
        }
        std::cout << std::endl;
    }
    return 0;
}