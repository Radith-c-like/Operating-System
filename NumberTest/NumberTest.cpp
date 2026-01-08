#include <iostream>
#include <cmath>
#include "Number.h"

int main() {

    std::cout << "1. Testing constructors:" << std::endl;
    Number defaultNum;
    std::cout << "   Default constructor: " << defaultNum.getValue();

    Number paramNum(42.5);
    std::cout << "   Parameterized constructor: " << paramNum.getValue();
    std::cout << std::endl;

    Number testNum;
    testNum.setValue(100.75);
    std::cout << "   After setValue(100.75): " << testNum.getValue();
    testNum.setValue(-33.25);
    std::cout << "   After setValue(-33.25): " << testNum.getValue();
    std::cout << std::endl;

    Number a(15.0);
    Number b(3.0);

    std::cout << "   a = " << a.getValue() << ", b = " << b.getValue() << std::endl;
    std::cout << "   a + b = " << (a + b).getValue() << " (expected: 18)" << std::endl;
    std::cout << "   a - b = " << (a - b).getValue() << " (expected: 12)" << std::endl;
    std::cout << "   a * b = " << (a * b).getValue() << " (expected: 45)" << std::endl;
    std::cout << "   a / b = " << (a / b).getValue() << " (expected: 5)" << std::endl;
    std::cout << std::endl;

    std::cout << "6. Testing global objects:" << std::endl;
    std::cout << "   NumberZero = " << NumberZero.getValue();
    std::cout << "   NumberOne = " << NumberOne.getValue();

    std::cout << "7. Testing CreateNumber function:" << std::endl;
    Number created = CreateNumber(7.7);
    std::cout << "   CreateNumber(7.7) = " << created.getValue();
    return 0;
}