#include <windows.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>

using namespace std;

static string formatSize(SIZE_T bytes) {
    const SIZE_T KiB = 1024;
    const SIZE_T MiB = 1024 * 1024;
    std::stringstream ss;
    ss << fixed << setprecision(2);
    if (bytes >= MiB) ss << (double)bytes / MiB << " MiB";
    else if (bytes >= KiB) ss << (double)bytes / KiB << " KiB";
    else ss << bytes << " B";
    return ss.str();
}

// Функция для вывода информации о куче
void HeapInfo(HANDLE heap) {
    PROCESS_HEAP_ENTRY entry;
    entry.lpData = nullptr;
    cout << "=== Heap Info ===" << endl;
    SIZE_T totalSize = 0;
    BOOL walked = FALSE;
    while (HeapWalk(heap, &entry)) {
        walked = TRUE;
        cout << "Address: " << entry.lpData
            << " | Size: " << entry.cbData << " (" << formatSize(entry.cbData) << ")"
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
    if (!walked) {
        DWORD err = GetLastError();
        if (err != ERROR_NO_MORE_ITEMS) {
            cerr << "HeapWalk error: " << err << endl;
        }
    }
    cout << "Total Heap Size (sum of regions): " << totalSize << " (" << formatSize(totalSize) << ")" << endl;
    cout << "=================" << endl;
}

int main(int argc, char* argv[]) {
    // Параметры по умолчанию (задание)
    int blockCount = 10;
    SIZE_T blockSize = 512 * 1024; // 512 KiB

    // Если указан режим test, меняем параметры (5 блоков по 1 MiB)
    bool testMode = false;
    if (argc > 1) {
        string arg = argv[1];
        if (arg == "test" || arg == "try" || arg == "t") testMode = true;
    }
    else {
        // интерактивный выбор
        cout << "Select mode: [1] default (10 x 512 KiB), [2] test (5 x 1 MiB): ";
        int sel = 1;
        if (!(cin >> sel)) sel = 1;
        if (sel == 2) testMode = true;
    }

    if (testMode) {
        blockCount = 5;
        blockSize = 1 * 1024 * 1024; // 1 MiB
        cout << "Mode: TEST (5 blocks x 1 MiB)" << endl;
    }
    else {
        cout << "Mode: DEFAULT (10 blocks x 512 KiB)" << endl;
    }

    // Этап 1: Создание пользовательской кучи
    HANDLE heap = HeapCreate(0, 1024 * 1024, 8 * 1024 * 1024);
    if (!heap) {
        cerr << "Heap creation error!" << endl;
        return 1;
    }
    cout << "Stage 1: Heap created (initial 1 MiB, max 8 MiB)" << endl;
    HeapInfo(heap);
    system("pause & cls");

    // Этап 2: Выделение блоков памяти
    vector<int*> blocks;
    for (int i = 0; i < blockCount; i++) {
        int* block = (int*)HeapAlloc(heap, HEAP_ZERO_MEMORY, blockSize);
        if (!block) {
            cerr << "Block allocation error #" << i << endl;
            // перед выходом освобождаем ранее выделенные блоки и кучу
            for (int j = 0; j < (int)blocks.size(); j++) HeapFree(heap, 0, blocks[j]);
            HeapDestroy(heap);
            return 1;
        }
        blocks.push_back(block);
        cout << "Stage 2: block allocated #" << i << " with size " << blockSize << " (" << formatSize(blockSize) << ")" << endl;
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