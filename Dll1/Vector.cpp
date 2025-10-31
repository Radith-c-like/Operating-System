#include "pch.h"
#include "Vector.h"
// Глобальные векторы
Vector g_origin(0.0, 0.0);
Vector g_one_one(1.0, 1.0);

Vector::Vector() : m_x(0.0), m_y(0.0) {}
Vector::Vector(double x, double y) : m_x(x), m_y(y) {}

double Vector::getX() const { return m_x; }
double Vector::getY() const { return m_y; }

double Vector::length() const {
    return std::sqrt(m_x * m_x + m_y * m_y);
}

double Vector::angle() const {
    return std::atan2(m_y, m_x);
}

Vector Vector::add(const Vector& other) const {
    return Vector(m_x + other.m_x, m_y + other.m_y);
}

void Vector::print() const {
    std::cout << "(" << m_x << ", " << m_y << ")\n";
}
