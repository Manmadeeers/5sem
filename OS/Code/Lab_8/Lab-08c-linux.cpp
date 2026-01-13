#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

using namespace std;

void wait_step(const char* msg) {
    cout << "\n=== " << msg << " ===\n"
         << " pmap -X " << getpid() << "\"\n";
    cout << "Нажмите Enter для продолжения...";
    cin.ignore();
}

int main() {
    const size_t BLOCK_SIZE = 512 * 1024; // 512 KiB
    const size_t INT_COUNT = BLOCK_SIZE / sizeof(int);
    const int NUM_BLOCKS = 10;

    void* blocks[NUM_BLOCKS];

    // --- ЭТАП 1: Выделение 10 блоков по 512 KiB ---
    cout << "Этап 1: Выделение 10 блоков памяти по 512 KiB\n";

    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = malloc(BLOCK_SIZE);
        if (!blocks[i]) {
            cerr << "Ошибка malloc на блоке " << i << endl;
            return 1;
        }
        cout << "Блок " << i << " выделен по адресу: " << blocks[i] << endl;
    }

    wait_step("Этап 1: Блоки выделены (RSS может увеличиться)");

    // --- ЭТАП 2: Заполнение каждого блока int-массивом ---
    cout << "Этап 2: Заполнение блоков\n";

    for (int i = 0; i < NUM_BLOCKS; i++) {
        int* arr = (int*)blocks[i];
        for (size_t j = 0; j < INT_COUNT; j++) {
            arr[j] = j;
        }

        cout << "Блок " << i << " заполнен. Примеры: [0]=" << arr[0]
             << ", [100]=" << arr[100]
             << ", [последний]=" << arr[INT_COUNT - 1] << endl;
    }

    wait_step("Этап 2: Данные записаны (RSS увеличится — выделяются физические страницы)");

    // --- ЭТАП 3: Освобождение всех блоков ---
    cout << "Этап 3: Освобождение блоков памяти\n";

    for (int i = 0; i < NUM_BLOCKS; i++) {
        free(blocks[i]);
        cout << "Блок " << i << " освобождён.\n";
    }

    wait_step("Этап 3: Блоки освобождены (но glibc может удерживать память)");

    // --- ЭТАП 4: "Уничтожение кучи" ---
    // В glibc куча — это sbrk + mmap области.
    // Освободить её вручную нельзя — делается через mallopt().
    cout << "Этап 4: Уничтожение кучи (heap trim)\n";

    malloc_trim(0);   // просит glibc вернуть память системе

    cout << "Запрошен trim кучи (glibc может вернуть большие mmap-блоки).\n";

    wait_step("Этап 4: Куча очищена через malloc_trim()");

    cout << "\nРабота завершена.\n";
    return 0;
}
