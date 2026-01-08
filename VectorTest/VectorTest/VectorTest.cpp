#include <iostream>
#include <cmath>
#include <cassert>
#include "Vector.h"

int main() {
    Vector v1;
    std::cout << "   Default constructor: (" << v1.getX() << ", " << v1.getY() << std::endl;
    Vector v2(3.5, 4.2);
    std::cout << "   Parameterized constructor: (" << v2.getX() << ", " << v2.getY();
    std::cout << std::endl;

    
    Vector v3(7.1, -2.3);
    std::cout << "   Vector(7.1, -2.3): getX() = " << v3.getX();
    std::cout << std::endl;

    
    Vector v4(3.0, 4.0);
    double len1 = v4.length();
    std::cout << "   Vector(3, 4).length() = " << len1;
    
    std::cout << "4. Testing angle() method:" << std::endl;
    Vector v7(1.0, 0.0);
    std::cout << "   Vector(1, 0).angle() = " << v7.angle() << std::endl;

    Vector v8(0.0, 1.0);
    std::cout << "   Vector(0, 1).angle() = " << v8.angle();
    Vector v9(-1.0, 0.0);
    std::cout << "   Vector(-1, 0).angle() = " << v9.angle() << std::endl;

    Vector a(2.0, 3.0);
    Vector b(1.0, 4.0);
    Vector sum = a.add(b);
    std::cout << "   (2, 3) + (1, 4) = (" << sum.getX() << ", " << sum.getY() << std::endl;
    
    std::cout << "   g_origin = ";
    g_origin.print();
    std::cout << "   Length of g_origin = " << g_origin.length() << std::endl;

    std::cout << "   g_one_one = ";
    g_one_one.print();
    std::cout << "   Length of g_one_one = " << g_one_one.length() << std::endl;

    return 0;
}