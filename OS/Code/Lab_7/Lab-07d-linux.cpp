#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <cstring>
#include <cstdlib>
#include <limits.h>

using namespace std;

bool IsPrime(unsigned long long n)
{
    if (n < 2) return false;
    for (unsigned long long i = 2; i * i <= n; ++i)
        if (n % i == 0) return false;
    return true;
}

void RunPrimeWorker(unsigned long long workSeconds)
{
    timespec start{}, now{};
    clock_gettime(CLOCK_MONOTONIC, &start);
    unsigned long long primeCount = 0;
    unsigned long long number = 2;
    while (true)
    {
        if (IsPrime(number))
        {
            cout << ++primeCount << ") " << number << endl;
        }
        number++;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed =
            (now.tv_sec - start.tv_sec) +
            (now.tv_nsec - start.tv_nsec) / 1e9;
        if (elapsed >= workSeconds)
            break;
    }
    clock_gettime(CLOCK_MONOTONIC, &now);
    double total =
        (now.tv_sec - start.tv_sec) +
        (now.tv_nsec - start.tv_nsec) / 1e9;
    cout << "Worked for " << total << " seconds\n";
    cout << "Numbers found: " << primeCount << endl;
    cin.get();
}

void StartChildProcess(const char* exePath, const char* seconds)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // дочерний процесс: запустить тот же исполняемый файл с аргументом
        execl(exePath, exePath, seconds, (char*)nullptr);
        perror("execl");
        _exit(127);
    }
    else if (pid < 0)
    {
        perror("fork");
    }
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        unsigned long long seconds = strtoull(argv[1], nullptr, 10);
        RunPrimeWorker(seconds);
        return 0;
    }

    // Получаем абсолютный путь к исполняемому файлу.
    // Если argv[0] уже абсолютный — используем его; иначе пытаемся получить realpath.
    char exeFullPath[PATH_MAX];
    if (argv[0][0] == '/')
    {
        strncpy(exeFullPath, argv[0], PATH_MAX-1);
        exeFullPath[PATH_MAX-1] = '\0';
    }
    else
    {
        if (realpath(argv[0], exeFullPath) == nullptr)
        {
            // если realpath не сработал, попробуем сформировать из cwd + argv[0]
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)))
            {
                snprintf(exeFullPath, PATH_MAX, "%s/%s", cwd, argv[0]);
            }
            else
            {
                // fallback: просто argv[0]
                strncpy(exeFullPath, argv[0], PATH_MAX-1);
                exeFullPath[PATH_MAX-1] = '\0';
            }
        }
    }

    cout << "Launch child processes\n";

    StartChildProcess(exeFullPath, "60");
    StartChildProcess(exeFullPath, "120");

    // дождёмся всех детей
    while (true)
    {
        pid_t w = wait(nullptr);
        if (w == -1) break;
    }

    cout << "All child processes finished\n";
    return 0;
}