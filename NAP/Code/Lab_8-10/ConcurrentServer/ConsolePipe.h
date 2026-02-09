#pragma once
#include "Global.h"
#include <algorithm>

// Поток ConsolePipe: обеспечивает управление сервером через именованные каналы
DWORD WINAPI ConsolePipe(LPVOID pPrm) {
	cout << "ConsolePipe работает\n";
	DWORD rc = 0;
	HANDLE hPipe;
	try
	{
		char rnpname[50];
		strcpy_s(rnpname, "\\\\.\\pipe\\");
		strcat_s(rnpname, npname);

		// Создание канала
		if ((hPipe = CreateNamedPipe(rnpname, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL)) == INVALID_HANDLE_VALUE)
			throw SetErrorMsgText("CreateNamedPipe:", GetLastError());

		TalkersCommand* pMainCommand = (TalkersCommand*)pPrm;

		while (*pMainCommand != EXIT) {
			// Ожидание подключения клиента управления (RConsole)
			if (!ConnectNamedPipe(hPipe, NULL)) {
				if (GetLastError() != ERROR_PIPE_CONNECTED)
					throw SetErrorMsgText("ConnectNamedPipe:", GetLastError());
			}

			char ReadBuf[50], WriteBuf[100];
			DWORD nBytesRead = 0, nBytesWrite = 0;
			bool clientConnected = true;

			while (*pMainCommand != EXIT && clientConnected) {
				if (*pMainCommand == GETCOMMAND) {
					// Чтение команды (кода)
					if (!ReadFile(hPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL) || nBytesRead == 0) {
						clientConnected = false;
						break;
					}

					int n = atoi(ReadBuf);
					bool isGlobalCommand = true;
					TalkersCommand SetCommand = GETCOMMAND;

					switch (n) {
					case 0: // START
						sprintf_s(WriteBuf, "START");
						SetCommand = START;
						break;
					case 1: // STOP
						sprintf_s(WriteBuf, "STOP");
						SetCommand = STOP;
						break;
					case 2: // EXIT
						sprintf_s(WriteBuf, "EXIT");
						SetCommand = EXIT;
						break;
					case 3: // STATISTICS
						sprintf_s(WriteBuf, "\nАктивных: %d\nОтказов: %d\nЗакрытых: %d\nЗагружено DLL: %zu\n",
							Accept, Fail, Finished, LoadedLibraries.size());
						isGlobalCommand = false;
						break;
					case 4: // WAIT
						sprintf_s(WriteBuf, "WAIT");
						SetCommand = WAIT;
						break;
					case 5: // SHUTDOWN
						sprintf_s(WriteBuf, "SHUTDOWN");
						SetCommand = SHUTDOWN;
						break;

					case 6: // LOAD_LIB (Задание 29)
					{
						char libName[256];
						isGlobalCommand = false;
						// Сообщаем RConsole, что готовы принять имя DLL
						sprintf_s(WriteBuf, "READY_FOR_LIBNAME");
						WriteFile(hPipe, WriteBuf, (DWORD)strlen(WriteBuf) + 1, &nBytesWrite, NULL);

						// Читаем имя библиотеки
						if (ReadFile(hPipe, libName, sizeof(libName), &nBytesRead, NULL) && nBytesRead > 0) {
							LibraryInfo li;
							li.name = libName;
							li.hModule = LoadLibraryA(li.name.c_str());
							if (li.hModule) {
								li.sssAddr = (HANDLE(*)(char*, LPVOID))GetProcAddress(li.hModule, "SSS");
								if (li.sssAddr) {
									EnterCriticalSection(&scLibraries);
									LoadedLibraries.push_back(li);
									LeaveCriticalSection(&scLibraries);
									sprintf_s(WriteBuf, "Успешно: %s загружена", libName);
									printf("ConsolePipe: LOAD_LIB %s\n", libName);
								}
								else {
									FreeLibrary(li.hModule);
									sprintf_s(WriteBuf, "Ошибка: в DLL нет функции SSS");
								}
							}
							else {
								sprintf_s(WriteBuf, "Ошибка: DLL не найдена");
							}
						}
						break;
					}

					case 7: // UNLOAD_LIB (Задание 29)
					{
						char libName[256];
						isGlobalCommand = false;
						sprintf_s(WriteBuf, "READY_FOR_LIBNAME");
						WriteFile(hPipe, WriteBuf, (DWORD)strlen(WriteBuf) + 1, &nBytesWrite, NULL);

						if (ReadFile(hPipe, libName, sizeof(libName), &nBytesRead, NULL) && nBytesRead > 0) {
							bool found = false;
							EnterCriticalSection(&scLibraries);
							for (auto it = LoadedLibraries.begin(); it != LoadedLibraries.end(); ++it) {
								if (it->name == libName) {
									FreeLibrary(it->hModule);
									LoadedLibraries.erase(it);
									found = true;
									break;
								}
							}
							LeaveCriticalSection(&scLibraries);

							if (found) {
								sprintf_s(WriteBuf, "Успешно: %s выгружена", libName);
								printf("ConsolePipe: UNLOAD_LIB %s\n", libName);
							}
							else {
								sprintf_s(WriteBuf, "Ошибка: библиотека не найдена в списке");
							}
						}
						break;
					}

					default:
						sprintf_s(WriteBuf, "Неизвестная команда");
						isGlobalCommand = false;
						break;
					}

					// Если команда глобальная (START/STOP/EXIT...), меняем состояние сервера
					if (isGlobalCommand) {
						*pMainCommand = SetCommand;
						printf("ConsolePipe: применена команда %s\n", WriteBuf);
					}

					// Отправка ответа клиенту
					if (!WriteFile(hPipe, WriteBuf, (DWORD)strlen(WriteBuf) + 1, &nBytesWrite, NULL)) {
						clientConnected = false;
					}
				}
				else {
					// Если сервер занят обработкой (не в состоянии GETCOMMAND), ждем
					Sleep(100);
				}
			}
			DisconnectNamedPipe(hPipe);
		}
		CloseHandle(hPipe);
		cout << "ConsolePipe остановлен" << endl;
	}
	catch (string ErrorPipeText) {
		cout << "ConsolePipe Error: " << ErrorPipeText << endl;
	}
	catch (...) {
		cout << "ConsolePipe: Непредвиденная ошибка" << endl;
	}
	ExitThread(rc);
}
