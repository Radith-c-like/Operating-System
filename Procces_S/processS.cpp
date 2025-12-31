#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line;
    long long total = 0;
    if (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        long long num;
        while (iss >> num) {
            total += num;
        }
        std::cout << total << std::endl;
    }
    return 0;
}