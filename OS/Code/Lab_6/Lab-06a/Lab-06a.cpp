#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct {
	char* ThreadName;
	int IterNum;
} parms;
CRITICAL_SECTION critSection;
DWORD WINAPI FuncForThread(LPVOID parm) {
	parms* par = (parms*)parm;
	const char* username = "User-ee4aed31";
	int usernameLength = strlen(username);

	for (int i = 0; i < par->IterNum; i++) {

		if (i == 29) {
			EnterCriticalSection(&critSection);
		}

		printf("%5s   - %7d     - %5c\n", par->ThreadName, i + 1, username[i % usernameLength]);
		Sleep(100);

		if (i == 59) {
			LeaveCriticalSection(&critSection);
		}
	}
	return 0;
}
int main(int argc, char* argv[]) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	InitializeCriticalSection(&critSection);

	parms parms1;
	parms1.ThreadName = (char*)"A";
	parms1.IterNum = 90;
	HANDLE A = CreateThread(
		NULL,
		0,
		FuncForThread,
		&parms1,
		0,
		NULL
	);
	if (A == NULL) {
		printf("Ошибка: не удалось создать поток A\n");
	}

	parms parms2;
	parms2.ThreadName = (char*)"B";
	parms2.IterNum = 90;
	HANDLE B = CreateThread(
		NULL,
		0,
		FuncForThread,
		&parms2,
		0,
		NULL
	);
	if (B == NULL) {
		printf("Ошибка: не удалось создать поток B\n");
	}

	parms parms3;
	parms3.ThreadName = (char*)"main";
	parms3.IterNum = 90;
	FuncForThread(&parms3);

	WaitForSingleObject(A, INFINITE);
	WaitForSingleObject(B, INFINITE);

	CloseHandle(A);
	CloseHandle(B);

	DeleteCriticalSection(&critSection);
}
