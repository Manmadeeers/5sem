#include <iostream>
#include <ctime>
#include <cstdint>

uint64_t getTimeMs()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1000ULL +
           static_cast<uint64_t>(ts.tv_nsec) / 1000000ULL;
}
int main()
{
    uint64_t startTime = getTimeMs();
    uint64_t currentTime;
    unsigned long long counter = 0;
    bool printed5 = false;
    bool printed10 = false;

    while (true)
    {
        counter++;
        currentTime = getTimeMs();
        uint64_t elapsed = currentTime - startTime;
        if (!printed5 && elapsed >= 5000)
        {
            std::cout << "Iterations after 5 seconds: " << counter << std::endl;
            printed5 = true;
        }

        if (!printed10 && elapsed >= 10000)
        {
            std::cout << "Iterations after 10 seconds: " << counter << std::endl;
            printed10 = true;
        }
        if (elapsed >= 15000)
        {
            std::cout << "Final iterations after 15 seconds: " << counter << std::endl;
            break;
        }
    }
    return 0;
}
