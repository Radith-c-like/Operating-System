#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>

using namespace std;

string GetProcessName(pid_t pid) {
    string path = "/proc/" + to_string(pid) + "/comm";
    ifstream file(path);
    string name;
    if (file >> name) return name;
    return "";
}

bool KillProcessById(pid_t pid) {
    if (kill(pid, SIGTERM) == 0) {
        printf("   Процесс %d успешно завершен (SIGTERM)\n", pid);
        return true;
    } else {
        if (kill(pid, SIGKILL) == 0) {
            printf("   Процесс %d принудительно завершен (SIGKILL)\n", pid);
            return true;
        }
    }
    perror("Ошибка завершения процесса");
    return false;
}

bool KillProcessesByName(const string& name) {
    DIR* dir = opendir("/proc");
    if (!dir) return false;

    struct dirent* entry;
    bool found = false;
    pid_t myPid = getpid();

    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) {
            pid_t pid = stoi(entry->d_name);
            if (pid == myPid) continue;

            if (GetProcessName(pid) == name) {
                printf("   Найден процесс: %s (PID: %d)\n", name.c_str(), pid);
                if (KillProcessById(pid)) found = true;
            }
        }
    }
    closedir(dir);
    if (!found) printf("Процессы '%s' не найдены\n", name.c_str());
    return found;
}

void PrintHelp() {
    printf("=== Linux Killer ===\n");
    printf("Использование:\n");
    printf("  ./killer --id <PID>\n");
    printf("  ./killer --name <имя>\n");
    printf("  ./killer (использует PROC_TO_KILL)\n");
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        char* env = getenv("PROC_TO_KILL");
        if (!env) {
            printf("Переменная PROC_TO_KILL не установлена\n");
            return 1;
        }
        string s(env);
        size_t pos = 0;
        while ((pos = s.find(',')) != string::npos) {
            KillProcessesByName(s.substr(0, pos));
            s.erase(0, pos + 1);
        }
        KillProcessesByName(s);
        return 0;
    } 
    
    if (argc == 3 && string(argv[1]) == "--id") {
        return KillProcessById(stoi(argv[2])) ? 0 : 1;
    } 
    
    if (argc == 3 && string(argv[1]) == "--name") {
        return KillProcessesByName(argv[2]) ? 0 : 1;
    }

    PrintHelp();
    return 0;
}