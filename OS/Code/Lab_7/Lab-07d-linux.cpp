#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <cstring>
#include <cstdlib>

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

void StartChildInNewTerminal(const char* seconds)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        execlp(
            "xterm",
            "xterm",
            "-hold",
            "-e",
            "./Lab-07d",
            seconds,
            nullptr
        );
        perror("execlp");
        exit(1);
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
    cout << "Launch child proccesses\n";

    StartChildInNewTerminal("60");
    StartChildInNewTerminal("120");

    while (wait(nullptr) > 0);

    cout << "All child processes finished\n";
    return 0;
}
