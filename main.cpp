#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

const int N = 15; // Ваш номер в списке

// Функции-обработчики для каждого процесса
void run_process_M() {
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
}

void run_process_A() {
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
}

void run_process_P() {
    std::string line;
    if (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        long long num;
        bool first = true;
        while (iss >> num) {
            if (!first) std::cout << " ";
            std::cout << (num * num * num);
            first = false;
        }
        std::cout << std::endl;
    }
}

void run_process_S() {
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
}

// Вспомогательная функция для создания процесса в цепочке
void spawn(void (*func)(), int in_fd, int out_fd) {
    pid_t pid = fork();
    if (pid == 0) { // Дочерний процесс
        if (in_fd != STDIN_FILENO) {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (out_fd != STDOUT_FILENO) {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        func();
        exit(0);
    }
}

int main() {
    std::cout << "Введите числа через пробел: ";
    std::string input;
    std::getline(std::cin, input);
    input += "\n";

    // Пайпы: fd[0] - чтение, fd[1] - запись
    int p1[2], p2[2], p3[2], p4[2], p5[2];
    pipe(p1); pipe(p2); pipe(p3); pipe(p4); pipe(p5);

    // Запускаем цепочку
    spawn(run_process_M, p1[0], p2[1]);
    spawn(run_process_A, p2[0], p3[1]);
    spawn(run_process_P, p3[0], p4[1]);
    spawn(run_process_S, p4[0], p5[1]);

    // Закрываем ненужные концы пайпов в родителе
    close(p1[0]); 
    close(p2[0]); close(p2[1]);
    close(p3[0]); close(p3[1]);
    close(p4[0]); close(p4[1]);
    close(p5[1]);

    // Отправляем данные
    write(p1[1], input.c_str(), input.length());
    close(p1[1]); // Сигнал конца данных

    // Читаем результат
    char buffer[256];
    ssize_t bytes = read(p5[0], buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::cout << "Результат: " << buffer;
    }

    close(p5[0]);

    // Ждем завершения всех детей
    for(int i = 0; i < 4; i++) wait(NULL);

    return 0;
}