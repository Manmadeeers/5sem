#include <iostream>
#include <unistd.h>      
#include <locale.h>      

// Глобальные переменные
int global_init = 4;                  
int global_uninit;                      
// Глобальные статические
static int static_global_init = 200;    
static int static_global_uninit
// Любая функция
void myFunction()
{
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    // Локальные переменные
    int local_init = 300;                   
    int local_uninit;                       
    // Локальные статические
    static int static_local_init = 400;     
    static int static_local_uninit;         

    std::cout << "=== Lab-08a ===\n";
    std::cout << "PID процесса: " << getpid() << "\n\n";

        std::cout << "Адрес функции myFunction:                 " << reinterpret_cast<void*>(myFunction) << '\n';
    std::cout << "Global initialized address:      " << &global_init << '\n';
    std::cout << "Global uninitialized address:    " << &global_uninit << '\n';
    std::cout << "Global static initialized address: " << &static_global_init << '\n';
    std::cout << "Global statuc uninitialized address: " << &static_global_uninit << '\n';

    std::cout << Local initialized address:       " << &local_init << '\n';
    std::cout << "Local uninitialized address:     " << &local_uninit << '\n';
    std::cout << "Local static initialized address: " << &static_local_init << '\n';
    std::cout << "Local static unitialized address: " << &static_local_uninit << '\n';

    std::cout << "argc address:                     " << &argc << '\n';
    std::cout << "argv address:                     " << &argv << '\n';

    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    return 0;
}
