#pragma once
#include "pch.h"
#include <Windows.h>
#define _CRT_SECURE_NO_WARNINGS

typedef unsigned int uint;
typedef unsigned long long ullong;


#define HTAPI __declspec(dllexport)



namespace HT {

	struct HTHANDLE    // блок управления HT
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
		HANDLE mutex_handle;            //Added: mutex handle for multi-process access
		char mutex_name[128];           //Added: name of global mutex for multi-process access
	};

	struct __declspec(dllexport) Element   // элемент 
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

	

	
	extern "C" {


		__declspec(dllexport)	HTHANDLE* Create   //  создать HT             
		(
			int	  Capacity,					   // емкость хранилища
			int   SecSnapshotInterval,		   // переодичность сохранения в сек.
			int   MaxKeyLength,                // максимальный размер ключа
			int   MaxPayloadLength,            // максимальный размер данных
			const char  FileName[512]          // имя файла 
		); 	// != NULL успешное завершение  

		__declspec(dllexport) HTHANDLE* Open     //  открыть HT             
		(
			const char    FileName[512]         // имя файла 
		); 	// != NULL успешное завершение  

		__declspec(dllexport) BOOL Snap         // выполнить Snapshot
		(
			HTHANDLE* hthandle           // управление HT (File, FileMapping)
		);

		__declspec(dllexport) BOOL Close        // Snap и закрыть HT  и  очистить HTHANDLE
		(
			HTHANDLE* hthandle           // управление HT (File, FileMapping)
		);	//  == TRUE успешное завершение   

		__declspec(dllexport) BOOL Insert(HTHANDLE* hthandle,  Element* element);

		__declspec(dllexport) BOOL Delete      // удалить элемент в хранилище
		(
			HTHANDLE* hthandle,            // управление HT (ключ)
			const Element* element              // элемент 
		);	//  == TRUE успешное завершение 

		__declspec(dllexport) Element* Get     //  читать элемент в хранилище
		(
			const HTHANDLE* hthandle,            // управление HT
			const Element* element              // элемент 
		); 	//  != NULL успешное завершение 

		__declspec(dllexport) BOOL Update     //  именить элемент в хранилище
		(
			const HTHANDLE* hthandle,            // управление HT
			const Element* oldelement,          // старый элемент (ключ, размер ключа)
			const void* newpayload,          // новые данные  
			int             newpayloadlength     // размер новых данных
		); 	//  != NULL успешное завершение 

		__declspec(dllexport) void Print                               // распечатать элемент 
		(
			const Element* element              // элемент 
		);

		__declspec(dllexport) const char* CreateSnapshotFileName(HTHANDLE* handle);

	}


};
