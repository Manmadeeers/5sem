#define _CRT_SECURE_NO_WARNINGS

#include "Global.h"
#include "AcceptServer.h"
#include "DispathServer.h"
#include "GarbageCleaner.h"
#include "ConsolePipe.h"
#include "ResponseServer.h"
#include "tchar.h"

int _tmain(int argc, _TCHAR* argv[]) {
	setlocale(LC_ALL, "Russian");

	try {
		// --- Обработка аргументов командной строки ---
		if (argc > 1) {
			int tmp = atoi(argv[1]);
			if (tmp >= 0 && tmp <= 65535) port = tmp;
			else cout << "Задан неверный TCP-порт" << endl;
		}
		cout << "TCP порт: " << port << endl;

		if (argc > 2) {
			int tmp = atoi(argv[2]);
			if (tmp >= 0 && tmp <= 65535) uport = tmp;
			else cout << "Задан неверный UDP-порт" << endl;
		}
		cout << "UDP порт: " << uport << endl;

		if (argc > 3) {
			dllname = argv[3]; // Имя стартовой библиотеки
		}

		if (argc > 4) {
			npname = argv[4];
		}
		cout << "Имя именованного канала: " << npname << endl;

		if (argc > 5) {
			ucall = argv[5];
		}
		cout << "Позывной сервера: " << ucall << endl;

		srand((unsigned)time(NULL));

		// --- Инициализация синхронизации ---
		InitializeCriticalSection(&scListContact);
		InitializeCriticalSection(&scLibraries); // Критическая секция для списка библиотек (Задание 29)

		// --- Загрузка первой библиотеки (по умолчанию) ---
		LibraryInfo firstLib;
		firstLib.name = dllname;
		firstLib.hModule = LoadLibraryA(firstLib.name.c_str());

		if (firstLib.hModule == NULL) {
			cout << "Ошибка при загрузке стартовой DLL: " << firstLib.name << endl;
		}
		else {
			firstLib.sssAddr = (HANDLE(*)(char*, LPVOID))GetProcAddress(firstLib.hModule, "SSS");
			if (firstLib.sssAddr == NULL) {
				cout << "Ошибка: функция SSS не найдена в " << firstLib.name << endl;
				FreeLibrary(firstLib.hModule);
			}
			else {
				EnterCriticalSection(&scLibraries);
				LoadedLibraries.push_back(firstLib);
				LeaveCriticalSection(&scLibraries);
				cout << "Успешно загружена стартовая DLL: " << firstLib.name << endl;
			}
		}

		volatile TalkersCommand cmd = START;

		// --- Создание потоков сервера ---
		hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
		HANDLE hDispathServer = CreateThread(NULL, NULL, DispathServer, (LPVOID)&cmd, NULL, NULL);
		HANDLE hConsolePipe = CreateThread(NULL, NULL, ConsolePipe, (LPVOID)&cmd, NULL, NULL);
		HANDLE hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);
		HANDLE hResponseServer = CreateThread(NULL, NULL, ResponseServer, (LPVOID)&cmd, NULL, NULL);

		// --- Настройка приоритетов ---
		SetThreadPriority(hAcceptServer, THREAD_PRIORITY_HIGHEST);
		SetThreadPriority(hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL);
		SetThreadPriority(hConsolePipe, THREAD_PRIORITY_NORMAL);
		SetThreadPriority(hDispathServer, THREAD_PRIORITY_NORMAL);
		SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_BELOW_NORMAL);

		// --- Ожидание завершения работы ---
		WaitForSingleObject(hAcceptServer, INFINITE);
		WaitForSingleObject(hDispathServer, INFINITE);
		WaitForSingleObject(hConsolePipe, INFINITE);
		WaitForSingleObject(hGarbageCleaner, INFINITE);
		WaitForSingleObject(hResponseServer, INFINITE);

		// --- Очистка ресурсов ---
		CloseHandle(hAcceptServer);
		CloseHandle(hDispathServer);
		CloseHandle(hGarbageCleaner);
		CloseHandle(hConsolePipe);
		CloseHandle(hResponseServer);

		DeleteCriticalSection(&scListContact);

		// Выгрузка всех загруженных библиотек (Задание 29)
		EnterCriticalSection(&scLibraries);
		for (auto& lib : LoadedLibraries) {
			if (lib.hModule) {
				FreeLibrary(lib.hModule);
				cout << "Выгружена библиотека: " << lib.name << endl;
			}
		}
		LoadedLibraries.clear();
		LeaveCriticalSection(&scLibraries);
		DeleteCriticalSection(&scLibraries);

	}
	catch (...) {
		cout << "Критическая ошибка в основном потоке сервера!" << endl;
	}

	cout << "Сервер завершил работу." << endl;
	system("pause");
	return 0;
}
