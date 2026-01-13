#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <cerrno>
#include <cstring>
#include <iomanip>
#include <cstdint>   
using namespace std;

void wait_step(const char* msg, pid_t pid) {
    cout << "\n=== " << msg << " ===\n"
         << " pmap -X " << pid << "\"\n"
         << "Нажмите Enter для продолжения..." << flush;
    string line;
    getline(cin, line); // надёжная пауза
}
void print_region(const char* name, void* start, size_t size_bytes, long page_size) {
    std::uintptr_t beg = reinterpret_cast<std::uintptr_t>(start);
    std::uintptr_t end = beg + size_bytes;

    cout << "\n--- " << name << " ---\n"
         << hex << showbase
         << " Начало (hex)     : " << beg << "\n"
         << " Конец (невкл.,hex): " << end << "\n"
         << dec << noshowbase
         << " Размер (байт)    : " << size_bytes << "\n"
         << " Размер (стр.)    : " << (page_size > 0 ? (size_bytes / page_size) : 0) << "\n"
         << "---------------------------\n";
}
int main() {
    errno = 0;
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        cerr << "Ошибка sysconf(_SC_PAGESIZE): " << strerror(errno) << "\n";
        return 1;
    }
    cout << "Размер страницы: " << page_size << " байт\n";
    // Константы задачи
    const size_t TOTAL_PAGES = 256;
    const size_t HALF_PAGES  = 128;

    const size_t TOTAL_SIZE = TOTAL_PAGES * static_cast<size_t>(page_size);
    const size_t HALF_SIZE  = HALF_PAGES  * static_cast<size_t>(page_size);

    pid_t pid = getpid();

    // --- ЭТАП 1: mmap PROT_NONE ---
    void* base = mmap(nullptr, TOTAL_SIZE, PROT_NONE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) {
        cerr << "mmap(PROT_NONE) failed: " << strerror(errno) << "\n";
        return 1;
    }
    print_region("Зарезервированная область (PROT_NONE)", base, TOTAL_SIZE, page_size);
    wait_step("Этап 1: Зарезервирована виртуальная память (RSS = 0, права ---)", pid);
    // --- ЭТАП 2: mprotect на вторую половину ---
    void* second_half = static_cast<char*>(base) + HALF_SIZE;

    cout << "\nАдрес второй половины: " << second_half << "\n"
         << "Смещение от начала   : +" << HALF_SIZE << " байт\n";

    if (mprotect(second_half, HALF_SIZE, PROT_READ | PROT_WRITE) != 0) {
        cerr << "mprotect(RW) failed: " << strerror(errno) << "\n";
        munmap(base, TOTAL_SIZE);
        return 1;
    }
    print_region("Вторая половина после mprotect(RW)", second_half, HALF_SIZE, page_size);
    wait_step("Этап 2: Вторая половина → READ/WRITE", pid);

    // --- ЭТАП 3: Запись массива ---
    size_t count_ints = HALF_SIZE / sizeof(int);
    int* arr = static_cast<int*>(second_half);

    for (size_t i = 0; i < count_ints; i++) {
        arr[i] = static_cast<int>(i);
    }

    cout << "\nВ массив записано чисел: " << count_ints << "\n";
    cout << "Контрольные элементы: [0] = " << arr[0]
         << ", [100] = " << arr[100]
         << ", [последний] = " << arr[count_ints - 1] << "\n";

    wait_step("Этап 3: Записаны данные (увеличится RSS, каждая запись → page fault)", pid);

    // --- ЭТАП 4: Сделать READ ONLY ---
    if (mprotect(second_half, HALF_SIZE, PROT_READ) != 0) {
        cerr << "mprotect(READ) failed: " << strerror(errno) << "\n";
        munmap(base, TOTAL_SIZE);
        return 1;
    }

    cout << "\nПрава второй половины изменены на READ ONLY.\n";
    print_region("Вторая половина после mprotect(READ)", second_half, HALF_SIZE, page_size);
    wait_step("Этап 4: READ ONLY (RSS останется прежним)", pid);

    // --- ЭТАП 5: Освобождение второй половины ---
    if (munmap(second_half, HALF_SIZE) != 0) {
        cerr << "munmap(2nd half) failed: " << strerror(errno) << "\n";
        munmap(base, TOTAL_SIZE);
        return 1;
    }

    cout << "\nОсвобождена вторая половина региона.\n";
    wait_step("Этап 5: Вторая половина исчезнет из pmap", pid);
// --- ЭТАП 6: Освобождение первой половины ---
    if (munmap(base, HALF_SIZE) != 0) {
        cerr << "munmap(1st half) failed: " << strerror(errno) << "\n";
        return 1;
    }
    cout << "\nОсвобождена первая половина.\n";
    wait_step("Этап 6: Полностью освобождена область памяти", pid);
    return 0;
}
