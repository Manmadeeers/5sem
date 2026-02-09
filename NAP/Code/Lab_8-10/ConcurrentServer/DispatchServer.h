#pragma once
#include "Global.h"

// DispathServer: поток-диспетчер
// Получает новых клиентов из очереди, считывает код запрашиваемого сервиса
// и ищет соответствующий обработчик во всех загруженных DLL.
DWORD WINAPI DispathServer(LPVOID pPrm) {
	cout << "DispathServer работает\n";
	DWORD rc = 0;
	try {
		while (*((TalkersCommand*)pPrm) != EXIT) {
			try {
				// Ожидаем уведомления о новом клиенте
				if (WaitForSingleObject(Event, 300) == WAIT_OBJECT_0) {

					if (ClientServiceNumber > 0) {
						EnterCriticalSection(&scListContact);

						for (ListContact::iterator p = Contacts.begin(); p != Contacts.end(); p++) {
							if (p->type == Contact::ACCEPT) {
								Contact* client = &(*p);
								char CallBuf[50] = "";
								int libuf = 0;
								bool received = false;

								// Читаем название сервиса (Rand, Time, Echo и т.д.)
								while (!received) {
									if ((libuf = recv(client->s, CallBuf, sizeof(CallBuf), NULL)) == SOCKET_ERROR) {
										if (WSAGetLastError() == WSAEWOULDBLOCK) Sleep(100);
										else throw SetErrorMsgText("Recv service name:", WSAGetLastError());
									}
									else received = true;
								}

								if (received && strlen(CallBuf) > 0) {
									client->type = Contact::CONTACT;
									strcpy_s(client->srvname, CallBuf);

									// Настройка таймера ограничения времени работы
									client->htimer = CreateWaitableTimer(NULL, false, NULL);
									_int64 time = -300000000; // 30 секунд
									SetWaitableTimer(client->htimer, (LARGE_INTEGER*)&time, 0, ASWTimer, client, false);

									// --- ПОИСК СЕРВИСА В ЗАГРУЖЕННЫХ БИБЛИОТЕКАХ (Задание 29) ---
									bool serviceStarted = false;

									EnterCriticalSection(&scLibraries);
									for (auto& lib : LoadedLibraries) {
										if (lib.sssAddr != NULL) {
											// Пытаемся запустить сервис из текущей DLL
											client->hthread = lib.sssAddr(CallBuf, client);

											if (client->hthread != NULL) {
												serviceStarted = true;
												cout << "Сервис [" << CallBuf << "] запущен из библиотеки: " << lib.name << endl;
												break; // Сервис найден и запущен, выходим из цикла по библиотекам
											}
										}
									}
									LeaveCriticalSection(&scLibraries);

									if (serviceStarted) {
										// Подтверждаем клиенту, что сервис запущен
										if (send(client->s, CallBuf, sizeof(CallBuf), NULL) == SOCKET_ERROR)
											throw SetErrorMsgText("Send confirm:", WSAGetLastError());
									}
									else {
										// Ни одна библиотека не опознала сервис
										cout << "Сервис [" << CallBuf << "] не найден ни в одной библиотеке." << endl;
										char SendError[50] = "Error: Service not found;";
										send(client->s, SendError, sizeof(SendError), NULL);

										closesocket(client->s);
										client->sthread = Contact::ABORT;
										CancelWaitableTimer(client->htimer);
										InterlockedIncrement(&Fail);
									}
								}
							}
						}
						LeaveCriticalSection(&scListContact);
					}
					SleepEx(0, true);
				}
				SleepEx(0, true);
			}
			catch (string errorMsgText) {
				cout << "DispathServer (loop): " << errorMsgText << endl;
				// В случае ошибки в конкретной итерации не выходим из потока
				if (scListContact.OwningThread == (HANDLE)GetCurrentThreadId()) LeaveCriticalSection(&scListContact);
			}
		}
	}
	catch (string errorMsgText) {
		cout << "DispathServer (fatal): " << errorMsgText << endl;
	}
	catch (...) {
		cout << "Error DispathServer" << endl;
	}
	cout << "DispathServer остановлен\n" << endl;
	ExitThread(rc);
}
