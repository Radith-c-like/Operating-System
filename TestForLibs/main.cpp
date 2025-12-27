#include <iostream>
#include "Vector.h"
#include "Number.h"

int main()
{
    std::cout << "=== Testing Vector DLL ===\n\n";

    // Используем глобальные векторы
    std::cout << "Global vectors:\n";
    std::cout << "g_origin = ";
    g_origin.print();

    std::cout << "g_one_one = ";
    g_one_one.print();

    // Создаем новые векторы
    Vector v1(3.0, 4.0);
    Vector v2(1.0, -2.0);

    std::cout << "\nCreated vectors:\n";
    std::cout << "v1 = ";
    v1.print();
    std::cout << "v2 = ";
    v2.print();

    // Сумма векторов
    Vector sum = v1.add(v2);
    std::cout << "\nSum (v1 + v2): ";
    sum.print();

    // Полярные координаты
    std::cout << "\nPolar coordinates of sum:\n";
    std::cout << "Length (r) = " << sum.length() << "\n";
    std::cout << "Angle (φ)  = " << sum.angle() << " radians\n";


    std::cout << "\n\n\n" << "============Test Number LIB=============" << '\n';

    Number a = CreateNumber(5.0);
    Number b = CreateNumber(2.0);

    Number diff = a - b;
    Number prod = a * b;
    Number quot = a / b;

    std::cout << "Diff: "; diff.print();
    std::cout << "Prod: "; prod.print();
    std::cout << "Quot: "; quot.print();

    std::cout << "Zero: "; NumberZero.print();
    std::cout << "One: "; NumberOne.print();

    return 0;
}
