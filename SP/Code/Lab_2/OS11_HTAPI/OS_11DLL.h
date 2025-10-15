#pragma once
#include <Windows.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>
#include <Windows.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <future>
#define METADATA_OFFSET 4*sizeof(int)+sizeof(time_t)

#ifdef HTLIB_EXPORTS
#define HTLIB_API __declspec(dllexport)
#else
#define HTLIB_API __declspec(dllimport)
#endif

extern "C" struct HTHANDLE    // блок управления HT
{
	HTHANDLE();
	HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
	int     Capacity;               // емкость хранилища в количестве элементов 
	int     SecSnapshotInterval;    // периодичность сохранения в сек. 
	int     MaxKeyLength;           // максимальная длина ключа
	int     MaxPayloadLength;       // максимальная длина данных
	char    FileName[512];          // имя файла 
	HANDLE  File;                   // File HANDLE != 0, если файл открыт
	HANDLE  FileMapping;            // Mapping File HANDLE != 0, если mapping создан  
	LPVOID  Addr;                   // Addr != NULL, если mapview выполнен  
	char    LastErrorMessage[512];  // сообщение об последней ошибке или 0x00  
	time_t  lastsnaptime;			// дата последнего snap'a (time())  
	int CurrentElements;            //Added: the number of elements in a storage
};

extern "C" struct Element   // элемент 
{
	Element();
	Element(const void* key, int keylength);                                             // for Get
	Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
	Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
	const void* key;                 // значение ключа 
	int             keylength;           // рахмер ключа
	const void* payload;             // данные 
	int             payloadlength;       // размер данных
};

extern "C" HTHANDLE* Create   //  создать HT             
(
	int	  Capacity,					   // емкость хранилища
	int   SecSnapshotInterval,		   // переодичность сохранения в сек.
	int   MaxKeyLength,                // максимальный размер ключа
	int   MaxPayloadLength,            // максимальный размер данных
	const char  FileName[512]          // имя файла 
); 	// != NULL успешное завершение  


extern "C" HTHANDLE* Open     //  открыть HT             
(
	const char    FileName[512]         // имя файла 
); 	// != NULL успешное завершение  


extern "C" BOOL Snap         // выполнить Snapshot
(
	HTHANDLE* hthandle           // управление HT (File, FileMapping)
);

extern "C" BOOL Close        // Snap и закрыть HT  и  очистить HTHANDLE
(
	HTHANDLE* hthandle           // управление HT (File, FileMapping)
);	//  == TRUE успешное завершение   

extern "C" BOOL Insert      // добавить элемент в хранилище
(
	HTHANDLE* hthandle,            // управление HT
	const Element* element              // элемент
);	//  == TRUE успешное завершение 

extern "C" BOOL Delete      // удалить элемент в хранилище
(
	HTHANDLE* hthandle,            // управление HT (ключ)
	const Element* element              // элемент 
);	//  == TRUE успешное завершение 


extern "C" Element* Get     //  читать элемент в хранилище
(
	const HTHANDLE* hthandle,            // управление HT
	const Element* element              // элемент 
); 	//  != NULL успешное завершение 


extern "C" BOOL Update     //  именить элемент в хранилище
(
	const HTHANDLE* hthandle,            // управление HT
	const Element* oldelement,          // старый элемент (ключ, размер ключа)
	const void* newpayload,          // новые данные  
	int             newpayloadlength     // размер новых данных
); 	//  != NULL успешное завершение 


extern "C" void Print                               // распечатать элемент 
(
	const Element* element              // элемент 
);


extern "C" const char* CreateSnapshotFileName(HTHANDLE* handle); //to create a snapshot file name that suits the folowing format: Snapshot-dd-mm-yyyy.htsnap


