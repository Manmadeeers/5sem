#include <stdio.h>
#include <tchar.h>
#include "iostream"
#include "Windows.h"                
#include "ctime"
#include <string>
using std::string;
using namespace std;

// Сформировать текст ошибки
string GetErrorMsgText(int code)
{
	if (code == 0) return std::string();

	char* messageBuffer = nullptr;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	string message(messageBuffer, size);
	LocalFree(messageBuffer);
	return message;
};

string SetPipeError(string msgText, int code)
{
	return msgText + to_string(code) + ". " + GetErrorMsgText(code);
};

int _tmain(int argc, _TCHAR* argv[]) {
	setlocale(LC_ALL, "Russian");

	char ReadBuf[100] = "";
	char WriteBuf[50] = "";
	DWORD nBytesRead;
	DWORD nBytesWrite;

#pragma region SecurityAttributes
	SECURITY_DESCRIPTOR* m_pSecDesc = (SECURITY_DESCRIPTOR*)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	SECURITY_ATTRIBUTES m_pSecAttrib = { sizeof(SECURITY_ATTRIBUTES), m_pSecDesc, TRUE };
	InitializeSecurityDescriptor(m_pSecDesc, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(m_pSecDesc, TRUE, (PACL)NULL, FALSE);
#pragma endregion

	int Code = 0;
	char serverName[256];
	char PipeName[512];

	try
	{
		printf_s("\n ---------- Доступные команды ---------- \n");
		printf_s("1 - start      \t (разрешить подключение клиентов)\n");
		printf_s("2 - stop       \t (запретить подключение клиентов)\n");
		printf_s("3 - exit       \t (завершить работу сервера)\n");
		printf_s("4 - statistics \t (вывод статистики)\n");
		printf_s("5 - wait       \t (ожидание завершения клиентов)\n");
		printf_s("6 - shutdown   \t (wait + exit)\n");
		printf_s("7 - load_lib   \t (загрузить новую DLL-библиотеку)\n"); // Задание 29
		printf_s("8 - unload_lib \t (выгрузить DLL-библиотеку)\n");        // Задание 29
		printf_s("0 - закрыть консоль\n");
		printf_s("\n ---------- ---------- ---------- ---------- \n");

		cout << "Введите имя сервера ('.' для локального): ";
		cin >> serverName;
		sprintf_s(PipeName, "\\\\%s\\pipe\\cpipe", serverName);

		// Подключение к именованному каналу
		HANDLE hNamedPipe = CreateFile(PipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, &m_pSecAttrib);
		if (hNamedPipe == INVALID_HANDLE_VALUE)
			throw SetPipeError("Ошибка подключения к каналу: ", GetLastError());

		do {
			printf_s("\nКоманда: ");
			if (!(cin >> Code)) {
				cin.clear();
				cin.ignore(1000, '\n');
				continue;
			}

			if (Code == 0) break;

			// Обработка стандартных команд (1-6) и новых команд (7-8)
			if (Code > 0 && Code < 9) {
				// Отправляем код команды (сервер ждет Code - 1)
				sprintf_s(WriteBuf, "%d", Code - 1);
				if (!WriteFile(hNamedPipe, WriteBuf, (DWORD)strlen(WriteBuf) + 1, &nBytesWrite, NULL))
					throw "WriteFile: Ошибка при отправке кода";

				// Для команд LOAD_LIB и UNLOAD_LIB требуется отправить имя библиотеки
				if (Code == 7 || Code == 8) {
					// Сначала читаем подтверждение готовности сервера (READY_FOR_LIBNAME)
					if (!ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL))
						throw "ReadFile: Ошибка ожидания готовности сервера";

					char libName[256];
					cout << "Введите имя библиотеки (напр. Service_Library.dll): ";
					cin >> libName;

					// Отправляем имя библиотеки
					if (!WriteFile(hNamedPipe, libName, (DWORD)strlen(libName) + 1, &nBytesWrite, NULL))
						throw "WriteFile: Ошибка при отправке имени библиотеки";
				}

				// Читаем финальный ответ от сервера
				if (!ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL))
					throw "ReadFile: Ошибка при чтении ответа";

				cout << "Сервер: " << ReadBuf << endl;
			}
			else {
				printf_s("Неверный код команды.\n");
			}
		} while (true);

		CloseHandle(hNamedPipe);
	}
	catch (string ErrorPipeText)
	{
		cout << endl << "Критическая ошибка: " << ErrorPipeText << endl;
	}
	catch (const char* msg) {
		cout << endl << msg << endl;
	}

	LocalFree(m_pSecDesc);
	cout << "RConsole остановлена\n";
	system("pause");
	return 0;
}
