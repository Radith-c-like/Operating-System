#include "pch.h"  // если используется в проекте Visual Studio
#include "Number.h"
#include <stdexcept>

Number::Number() : m_value(0.0) {}

Number::Number(double value) : m_value(value) {}

double Number::getValue() const {
    return m_value;
}

void Number::setValue(double value) {
    m_value = value;
}

Number Number::operator+(const Number& other) const {
    return Number(m_value + other.m_value);
}

Number Number::operator-(const Number& other) const {
    return Number(m_value - other.m_value);
}

Number Number::operator*(const Number& other) const {
    return Number(m_value * other.m_value);
}

Number Number::operator/(const Number& other) const {
    if (other.m_value == 0.0)
        throw std::runtime_error("Division by zero");
    return Number(m_value / other.m_value);
}

void Number::print() const {
    std::cout << m_value << std::endl;
}

// Глобальные переменные
Number NumberZero(0.0);
Number NumberOne(1.0);

// Функция создания числа
Number CreateNumber(double value) {
    return Number(value);
}
