#include <windows.h>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

// Функция для вывода информации о куче
void HeapInfo(HANDLE heap) {
    PROCESS_HEAP_ENTRY entry;
    entry.lpData = nullptr;

    cout << "=== Heap Info ===" << endl;

    SIZE_T totalSize = 0;
    while (HeapWalk(heap, &entry)) {
        cout << "Address: " << entry.lpData
            << " | Size: " << entry.cbData
            << " | Type: ";

        if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
            cout << "Busy ";
        else if (entry.wFlags & PROCESS_HEAP_ENTRY_MOVEABLE)
            cout << "Moveable";
        else
            cout << "Free ";

        cout << endl;
        totalSize += entry.cbData;
    }

    cout << "Total Heap Size: " << totalSize << " bytes" << endl;
    cout << "=================" << endl;
}

int main() {
    // Этап 1: Создание пользовательской кучи
    HANDLE heap = HeapCreate(0, 1024 * 1024, 8 * 1024 * 1024);
    if (!heap) {
        cerr << "Heap creation error!" << endl;
        return 1;
    }
    cout << "Stage 1: Heap created" << endl;
    HeapInfo(heap);
    system("pause & cls");

    // Этап 2: Выделение блоков памяти
    const int blockCount = 10;          // можно изменить на 5
    const SIZE_T blockSize = 512 * 1024; // можно изменить на 1 МиБ
    vector<int*> blocks;

    for (int i = 0; i < blockCount; i++) {
        int* block = (int*)HeapAlloc(heap, HEAP_ZERO_MEMORY, blockSize);
        if (!block) {
            cerr << "Block allocation error " << i << endl;
            return 1;
        }
        blocks.push_back(block);
        cout << "Stage 2: block allocated #" << i << " with size " << blockSize << " bytes" << endl;
        HeapInfo(heap);
        system("pause & cls");
    }

    // Этап 3: Заполнение блоков числами
    SIZE_T intCount = blockSize / sizeof(int);
    for (int i = 0; i < blockCount; i++) {
        for (SIZE_T j = 0; j < intCount; j++) {
            blocks[i][j] = (int)j;
        }
    }
    cout << "Stage 3: Blocks filled with numbers" << endl;
    system("pause & cls");

    // Этап 4: Освобождение памяти
    for (int i = 0; i < blockCount; i++) {
        HeapFree(heap, 0, blocks[i]);
        cout << "Stage 4: Block freed #" << i << endl;
    }
    HeapInfo(heap);
    system("pause & cls");

    // Этап 5: Уничтожение кучи
    if (HeapDestroy(heap)) {
        cout << "Stage 5: Heap destroyed" << endl;
    }
    else {
        cerr << "Heap destruction error!" << endl;
    }

    return 0;
}
