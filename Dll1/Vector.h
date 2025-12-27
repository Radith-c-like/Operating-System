#pragma once
#include <cmath>
#include <iostream>

#ifdef VECTOR_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif

class VECTOR_API Vector
{
private:
    double m_x;
    double m_y;

public:
    Vector();
    Vector(double x, double y);

    double getX() const;
    double getY() const;

    // Полярные координаты
    double length() const; // r = sqrt(x^2 + y^2)
    double angle() const;  // φ = atan2(y, x)

    // Сложение векторов
    Vector add(const Vector& other) const;

    void print() const;
};

// Глобальные векторы
extern "C" {
    VECTOR_API extern Vector g_origin;   // (0, 0)
    VECTOR_API extern Vector g_one_one;  // (1, 1)
}
