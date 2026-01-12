#include <iostream>
#include <windows.h>

int g_init = 1;
int g_uninit;

static int sg_init = 2;
static int sg_uninit;

void my_function() {}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    int local;
    int local_init = 3;

    static int s_local_init = 4;
    static int s_local_uninit;

    std::cout << "Global initialized: " << (void*)&g_init << std::endl;
    std::cout << "Global uninitialized: " << (void*)&g_uninit << std::endl;
    std::cout << "Global static initialized: " << (void*)&sg_init << std::endl;
    std::cout << "Global static unitialized: " << (void*)&sg_uninit << std::endl;

    std::cout << "Function my_function: " << (void*)&my_function << std::endl;

    std::cout << "Local: " << (void*)&local << std::endl;
    std::cout << "Local initialized: " << (void*)&local_init << std::endl;

    std::cout << "Local static initialized: " << (void*)&s_local_init << std::endl;
    std::cout << "Local static unitialized: " << (void*)&s_local_uninit << std::endl;

    std::cout << "argc: " << (void*)&argc << std::endl;
    std::cout << "argv: " << (void*)&argv << std::endl;

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}
