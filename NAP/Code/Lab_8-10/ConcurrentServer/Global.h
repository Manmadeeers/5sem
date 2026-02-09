//Global
#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "locale.h"
#include <iostream>
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")
#include "Windows.h"
#include <string>
#include <list>
#include <vector>
#include <time.h>
#include <tchar.h>
#include "Error.h"

using std::string;
using std::list;

// --- Параметры сервера ---
int port = 2000;                  // используемый TCP-порт
int uport = 2000;                 // UDP-порт для ResponseServer
const char* dllname = "Service_Library"; // название первой библиотеки по умолчанию
const char* npname = "cpipe";     // имя именованного канала
const char* ucall = "HELLO";      // позывной сервера

// --- Управление библиотеками (Задание №29) ---
struct LibraryInfo {
    string name;                               // Имя файла DLL
    HMODULE hModule;                           // Дескриптор загруженной DLL
    HANDLE(*sssAddr)(char*, LPVOID);           // Указатель на функцию SSS в этой DLL
};

list<LibraryInfo> LoadedLibraries;             // Список всех загруженных библиотек
CRITICAL_SECTION scLibraries;                  // Критическая секция для списка библиотек

// --- Системные объекты ---
HANDLE hAcceptServer;                          // Дескриптор потока AcceptServer
HANDLE Event = CreateEvent(NULL, FALSE, FALSE, NULL); // Событие для DispatchServer
int AS_SQUIRT = 10;                            // Максимальное кол-во клиентов

// --- Команды управления ---
enum TalkersCommand {
    START, STOP, EXIT, STATISTICS, WAIT, SHUTDOWN, GETCOMMAND,
    LOAD_LIB,   // Загрузить новую DLL
    UNLOAD_LIB  // Выгрузить существующую DLL
};

// --- Статистика ---
volatile LONG Accept = 0;               // Кол-во подключений
volatile LONG Fail = 0;                 // Неудачные/отказанные
volatile LONG Finished = 0;             // Успешно завершенные
volatile LONG ClientServiceNumber = 0;  // Активные в данный момент

#pragma region ListContact

CRITICAL_SECTION scListContact; // Критическая секция для списка клиентов

struct Contact {
    enum TE { EMPTY, ACCEPT, CONTACT } type;
    enum ST { WORK, ABORT, TIMEOUT, FINISH } sthread;

    SOCKET      s;
    SOCKADDR_IN prms;
    int         lprms;

    HANDLE      hthread;   // Дескриптор потока из DLL
    HANDLE      htimer;    // Дескриптор таймера

    bool        TimerOff;
    bool        CloseConn;

    char msg[50];
    char srvname[15];

    HANDLE hAcceptServer;

    Contact(TE t = EMPTY, const char* namesrv = "") {
        memset(&prms, 0, sizeof(SOCKADDR_IN));
        lprms = sizeof(SOCKADDR_IN);
        type = t;
        strcpy_s(srvname, namesrv);
        msg[0] = 0;
        CloseConn = false;
        TimerOff = false;
        hthread = NULL;
        htimer = NULL;
    };

    void SetST(ST sth, const char* m = "") {
        sthread = sth;
        strcpy_s(msg, m);
    }
};

typedef list<Contact> ListContact;
ListContact Contacts;

#pragma endregion

// --- Асинхронные функции (APC) ---

void CALLBACK ASWTimer(LPVOID Lprm, DWORD, DWORD) {
    char obuf[50] = "Close: timeout;";
    Contact* client = (Contact*)Lprm;

    EnterCriticalSection(&scListContact);
    client->TimerOff = true;
    client->sthread = Contact::TIMEOUT;
    LeaveCriticalSection(&scListContact);

    send(client->s, obuf, sizeof(obuf), NULL);

    SYSTEMTIME stt;
    GetLocalTime(&stt);
    printf("%d.%d.%d %d:%02d Timeout %s\n", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute, client->srvname);
}

void CALLBACK ASStartMessage(DWORD Lprm) {
    Contact* client = (Contact*)Lprm;
    SYSTEMTIME stt;
    GetLocalTime(&stt);
    printf("%d.%d.%d %d:%02d %s started;\n", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute, client->srvname);
}

void CALLBACK ASFinishMessage(DWORD Lprm) {
    Contact* client = (Contact*)Lprm;
    SYSTEMTIME stt;
    GetLocalTime(&stt);
    printf("%d.%d.%d %d:%02d %s stopped;\n", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute, client->srvname);
}
