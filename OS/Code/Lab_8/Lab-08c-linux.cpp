Код:

```cpp
#include <iostream>
#include и соберите:

g++ -std=c++17 -O2 -Wall -Wextra -o заголовка <malloc.h> на Linux.

Ниже — исправленный иcpp
#include <iostream>
#include <cstdlib>
#include <cstring>
 <cstdlib>
#include <cstring>
#include <unistd.h>
#include <malloc.h>   // для mem_demo mem_demo.cpp

Исправленный код:

```cpp
#include <iostream слегка улучшенный вариант вашей программы. Я:
- удалил лишнюю кавычку в#include <unistd.h>
#include <malloc.h>        // для malloc_trim на Linux
 malloc_trim на Linux

using namespace std>
#include <cstdlib>
#include < wait_step;
- подключил <malloc.h> для;

void wait_step(const char* msg) {
    cout << "\n=== " << msg << " ===\n"
         <<cstring>
#include <unistd.h>
#include <malloc.h>   // <-- для malloc_trim
#include <limits>

using namespace std;

void wait_step(const char#include <limits>          // для numeric_limits

using namespace std;

void wait_step(const char* msg) {
    cout << "\n=== " << msg << " === malloc_trim;
- заменил cin.ignore() на cin.get() (чтобы надёжно ждать Enter);
- привёл некоторые приведения типов к " pmap -X " << getpid() << "\n";
    cout << "Нажмите Enter* msg) {
    cout << "\n=== " << msg << " ===\n"
         << "Ком\n"
         << " pmap -X " << getpid() << "\n";
    cout << "Нажмите Enter C++-стилю и аккуратно отформатировал вывод.

Сохраните в файл (например, для продолжения...";
    cin.get();
}

int main() {
анда для просмотра карт памяти процесса:    const size_t BLOCK_SIZE = 512 * 1024; // 512 KiB для продолжения..." << flush;
    // читаем до конца строки, чтобы pmap -X " << getpid() << "\n";
    cout << "Нажмите Enter для продолжения...";
    string tmp memory_demo.cpp) и соберите:
g++ -std=c++17 -O2 -Wall -Wextra -o memory_demo memory_demo.cpp

Код:

```cpp
    const size_t INT_COUNT = BLOCK_SIZE / sizeof(int);
    const int NUM_BLOCKS = 10;
    void ждать Enter
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    const size_t BLOCK_SIZE = 512;
    std::getline(cin, tmp);
}

int main() {
    const size_t BLOCK_SIZE = 512
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
* blocks[NUM_BLOCKS];

    // --- ЭТАП  * 1024; // 512 KiB * 1024; // 512 KiB
    const size_t INT_COUNT = BLOCK_SIZE /#include <malloc.h>   // для malloc_trim на Linux

using namespace std;

void wait_step(const char* msg) {

    const size_t INT_COUNT = BLOCK_SIZE / sizeof(int);
    const int NUM_BLOCKS = 1: Выделение 10 блоков по 512 KiB ---
    cout << "Этап  sizeof(int);
    const int NUM_BLOCKS = 10;

    void* blocks[NUM_BLOCK10;
    void* blocks[NUM_BLOCKS] = { nullptr };

    //1: Выделение 10 блоков памяти по 512 KiB\n";
    for (    cout << "\n=== " << msg << " ===\n"
         << " pmap -X " << getS];

    // --- ЭТАП 1: Выделение --- ЭТАП 1: Выделение 10int i = 0; i < NUM_BLOCKS; ipid() << "\n";
    cout << "Нажмите Enter 10 блоков по 512 KiB ---
    cout << "Этап 1 блоков по 512 KiB ---
    cout << "Этап 1: Выдел++) {
        blocks[i] = malloc(BLOCK_SIZE);
        if (!blocks[i]) {
            для продолжения...";
    cin.get();
}

int main() {
    const size_t BLOCK: Выделение 10 блоков памяти по 512 KiB\n";
    forение 10 блоков памяти по 512 KiB\n";
    for (int i = 0 cerr << "Ошибка malloc на блоке " << i << endl;
            return 1;
        }
       _SIZE = 512 * 1024; // 512 KiB
    const size_t INT_COUNT = (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = malloc; i < NUM_BLOCKS; i++) {
        blocks[i] = malloc(BLOCK cout << "Блок " << i << " выделен по адресу: " BLOCK_SIZE / sizeof(int);
    const int NUM(BLOCK_SIZE);
        if (!blocks[i]) {
            cerr << "Ошибка_SIZE);
        if (!blocks[i]) {
            cerr << "_BLOCKS = 10;

    void* blocks[NUM_BLOCKS];

    // --- ЭТА << blocks[i] << endl;
    }

    wait_step("Этап 1: Блоки выделены (RSS может увеличиться)");

   Ошибка malloc на блоке " << i << endl;
            return 1;
        }
        cout << "Блок malloc на блоке " << i << endl;
            return 1;
        }
        cout << "Блок " << i << " выделП 1: Выделение 10 блоков по 512 KiB ---
    cout << "Этап  // --- ЭТАП 2: Заполнение каждого блока int-масс " << i << " выделен по адресу: " << blocks[i] << '\n';
    }

   ен по адресу: " << blocks[i] << endl;
    }

    wait_step("Этап 1: Б1: Выделение 10 блоков памяти по 512 KiB\n";
    for (int i = 0ивом ---
    cout << "Этап 2: Заполнение блоков\n";
    for (int i = 0; i wait_step("Этап 1: Блоки выделены (RSS может увеличиться)");

    // --- Элоки выделены (RSS может увеличиться)");

    // --- ЭТАП 2: Заполнение каждого; i < NUM_BLOCKS; i++) {
        blocks[i] = std::malloc(BLOCK_SIZE < NUM_BLOCKS; i++) {
        int* arr = static_cast<int*>(blocks[iТАП 2: Заполнение каждого блока int-массивом ---
    cout блока int-массивом ---
    cout << "Этап 2);
        if (!blocks[i]) {
            cerr << "Ошибка]);
        for (size_t j = 0; j < INT_COUNT; << "Этап 2: Заполнение блоков\n";
    for (int i =: Заполнение блоков\n";
    for (int i = 0; i < NUM_BLOCKS malloc на блоке " << i << endl;
            return 1;
        }
        cout << "Блок " j++) {
            arr[j] = static_cast<int>(j);
        }
        cout << " 0; i < NUM_BLOCKS; i++) {
        int* arr; i++) {
        int* arr = static_cast<int*>(blocks[i]);
 << i << " выделен по адресу: " << blocks[i] <<Блок " << i << " заполнен. Примеры: [0 = static_cast<int*>(blocks[i]);
        for (size_t j = 0; j        for (size_t j = 0; j < INT_COUNT endl;
    }

    wait_step("Э]=" << arr[0]
             << ", [100]=" << arr[100]
             << < INT_COUNT; j++) {
            arr[j] = static_cast<int>(j);
       тап 1: Блоки выделены (RSS может увеличиться)");

    // --- ЭТАП ; j++) {
            arr[j] = static_cast<int>(j);
        }
        cout << "Блок " << i << ", [последний]=" << arr[INT_COUNT -  }
        cout << "Блок " << i << " заполнен. Пр2: Заполнение каждого блока int-массивом ---
    cout << "Э " заполнен. Примеры: [0]=" << arr[0]
            1] << endl;
    }

    wait_step("Этап 2: Данные записаны (имеры: [0]=" << arr[0]
             << ", [100]="тап 2: Заполнение блоков\n";
    for (int i = << ", [100]=" << arr[100]
             << ", [последнийRSS увеличится — выделяются физические страницы)");

    // --- ЭТАП 3: << arr[100]
             << ", [последний]=" << arr 0; i < NUM_BLOCKS; i++)]=" << arr[INT {
        int* arr = static_cast<int*>(blocks[i]);
        for (size_t j = 0; j < INT_COUNT; j++) {
            arr[j][INT_COUNT - 1] << '\n';
    }

    wait_step("Этап 2: Данные записаны (RSS увеличится — выделяются физические страницы)");

    // --- ЭТАП _COUNT - 1] << endl;
    }

    wait_step("Этап 2: Данные записаны (RSS увеличится — выделяются физические страницы)");

    // --- ЭТАП 3: Освобождение всех блоков Освобождение всех блоков ---
    cout << "Этап 3: Освобождение блоков памяти\n";
    for (int i = 0; i < NUM_BLOCKS; i++) {
        free(blocks[i]);
        blocks[i] = = static_cast<int>(j);
        }
        cout << "Блок " << i << " заполнен. Примеры: [03: Освобождение всех блоков ---
    cout << "Этап 3: Освобождение блоков памяти\n";
    ---
    cout << "Этап 3: Освобождение блоков памяти\n";
    for (int i = 0; nullptr;
        cout << "Блок " << i << " освобождён.\n";
    }

    wait_step("]=" << arr[0]
             << ", [100]=" << arr[100]
 for (int i = 0; i < NUMЭтап 3:             << ", [последний]=" << arr[INT_COUNT - 1] << endl;
    }

   _BLOCKS; i++) {
        free(blocks[i]);
        blocks[i] = nullptr;
        cout << "Блок " << i << " освобождён.\ i < NUM_BLOCKS; i++) {
        free(blocks[i]);
        blocks[i] = nullptr;
        cout << "Блок " << i << " освобождён.\n";
    }

    wait_step Блоки освобождены (но glibc может удерживать память)");

    // --- ЭТАП 4: "Уничтожение кучи" ---
    cout << "Этап 4: Уничтожение wait_step("Этап 2: Данные записаны (RSS увеличится — выделяются физические страницы)");

    // --- ЭТАП 3:n";
    }

    wait_step("Этап 3: Блоки освобождены (но glibc может("Этап 3: Блоки освобождены (но glibc может удерживать память кучи (heap trim)\n";
    if (malloc_trim(0) != 0) {
        cout Освобождение всех блоков ---
    cout << "Этап 3: Освобождение блок удерживать память)");

    // --- ЭТАП 4: "Уничтожение кучи" (heap trim) ---
)");

    // --- ЭТАП 4: "Уничтожение кучи" ---
    cout << " << "malloc_trim вернул успех (попытка вернуть память системе).\ов памяти\n";
    for (int i =     cout << "Этап 4: Уничтожение кучи (heap trim)\n";
    // malloc_trim возвращает 0; i < NUM_BLOCKS; i++) {
        std::free(blocks[i]);
        cout << "Блок " << i <<Этап 4: Уничтожение кучи (heap trim)\n";
    // malloc_trim возвращает 1, если удалось вернуть память ядру, n";
    } else {
        cout << "malloc_trim вернул 0 (ничего не было возвращено).\n";
    }

    wait_step("Этап 4:1 если удалось вернуть память ядру, 0 иначе
    int trimmed = malloc_trim( " освобождён.\n";
    }

    wait_step("Этап 3:0 — иначе.
    int trimmed = malloc_trim(0);
    cout << "malloc Куча очищена через malloc_trim()");

    cout << "\0);
    cout << "Результат malloc Блоки освобождены (но glibc может удерживать память)");

    // ---_trim(0) вернул: " << trimmed << " (1 — память возвращ_trim: " << trimmed << " (1 — возврат возможен)\n";
    coutnРабота завершена.\n";
    return 0;
}