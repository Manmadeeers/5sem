#include <iostream>
#include <unistd.h>
#include <locale.h>

int global_init = 4;
int global_uninit;

static int static_global_init = 200;
static int static_global_uninit; // added missing semicolon

void myFunction()
{
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    int local_init = 300;
    int local_uninit;

    static int static_local_init = 400;
    static int static_local_uninit;

    std::cout << "=== Lab-08a ===\n";
    std::cout << "PID процесса: " << getpid() << "\n\n";

    // Печать адреса функции: приведение к void* ради вывода (диагностическая информация)
    std::cout << "Адрес функции myFunction:                   "
              << reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&myFunction)) << '\n';

    std::cout << "Global initialized address:                 " << static_cast<const void*>(&global_init) << '\n';
    std::cout << "Global uninitialized address:               " << static_cast<const void*>(&global_uninit) << '\n';
    std::cout << "Global static initialized address:          " << static_cast<const void*>(&static_global_init) << '\n';
    std::cout << "Global static uninitialized address:        " << static_cast<const void*>(&static_global_uninit) << '\n';
    std::cout << "Local initialized address:                  " << static_cast<const void*>(&local_init) << '\n';
    std::cout << "Local uninitialized address:                " << static_cast<const void*>(&local_uninit) << '\n';
    std::cout << "Local static initialized address:           " << static_cast<const void*>(&static_local_init) << '\n';
    std::cout << "Local static uninitialized address:         " << static_cast<const void*>(&static_local_uninit) << '\n';
    std::cout << "argc address:                               " << static_cast<const void*>(&argc) << '\n';
    std::cout << "argv address:                               " << static_cast<const void*>(&argv) << '\n';

    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    return 0;
}