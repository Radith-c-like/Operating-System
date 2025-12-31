#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

bool IsProcessRunning(pid_t pid) {
    if (pid <= 0) return false;
    return kill(pid, 0) == 0;
}

pid_t StartProcess(const string& command, const string& appName) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp(command.c_str(), command.c_str(), "-u", "NONE", nullptr);
        exit(0); 
    } else {
        printf("   Запущен %s (PID: %d)\n", appName.c_str(), pid);
        return pid;
    }
}

int main() {
    printf("=== Демонстрация Killer для Linux ===\n\n");

    setenv("PROC_TO_KILL", "vim,nano", 1);

    printf("1. Запуск тестовых процессов...\n");
    pid_t pidVim = StartProcess("vim", "Vim");
    sleep(1);
    pid_t pidNano = StartProcess("nano", "Nano");
    sleep(2);

    printf("\n2. Тест: Завершение Vim по ID (PID: %d)\n", pidVim);
    string cmd = "./killer --id " + to_string(pidVim);
    system(cmd.c_str());
    
   sleep(2);
    printf("   Статус Vim: %s\n", IsProcessRunning(pidVim) ? "Все еще запущен" : "Успешно убит");

    printf("\n3. Тест: Завершение через PROC_TO_KILL (ищем Nano)\n");
    system("./killer"); 
    
    sleep(2);
    printf("   Статус Nano: %s\n", IsProcessRunning(pidNano) ? "Все еще запущен" : "Успешно убит");

    printf("\nДемонстрация окончена. Нажмите Enter...");
    cin.get();
    return 0;
}