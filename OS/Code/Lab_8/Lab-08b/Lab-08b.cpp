#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    SIZE_T pageSize = si.dwPageSize;

    SIZE_T reserveSize = 256 * pageSize;

    LPVOID baseAddr = VirtualAlloc(NULL, reserveSize, MEM_RESERVE, PAGE_READWRITE);
    std::cout << "Stage 1: 256 pages reserved. Address: " << baseAddr << std::endl;
    std::cin.get();

    LPVOID commitAddr = (LPBYTE)baseAddr + 128 * pageSize;
    VirtualAlloc(commitAddr, 128 * pageSize, MEM_COMMIT, PAGE_READWRITE);
    std::cout << "Stage 2: Physical memory for the second part allocated" << std::endl;
    std::cin.get();

    int* arr = (int*)commitAddr;
    for (int i = 0; i < (128 * pageSize) / sizeof(int); i++) {
        arr[i] = i;
    }
    std::cout << "Stage 3: Memory is filled with numbers starting with 0 and incrementing by 1" << std::endl;
    std::cin.get();

    DWORD oldProtect;
    VirtualProtect(commitAddr, 128 * pageSize, PAGE_READONLY, &oldProtect);
    std::cout << "Stage 4:Security attributes changed to 'Read Only'." << std::endl;
    std::cin.get();

    VirtualFree(commitAddr, 128 * pageSize, MEM_DECOMMIT);
    std::cout << "Stage 5: Physical memory for the second part freed" << std::endl;
    std::cin.get();

    VirtualFree(baseAddr, 0, MEM_RELEASE);
    std::cout << "Stage 6: All virtual memory freed" << std::endl;
    std::cin.get();

    return 0;
}
