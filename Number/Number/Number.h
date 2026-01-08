#pragma once
#include <iostream>

class Number
{
private:
    double m_value;

public:
    Number();
    Number(double value);

    double getValue() const;
    void setValue(double value);

    // Арифметические операции
    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;

    // Вывод
    void print() const;
};

// Глобальные переменные
extern Number NumberZero;
extern Number NumberOne;

// Функция создания числа
Number CreateNumber(double value);
