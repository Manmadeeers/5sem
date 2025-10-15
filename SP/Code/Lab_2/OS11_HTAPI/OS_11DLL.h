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

extern "C" struct HTHANDLE    // ���� ���������� HT
{
	HTHANDLE();
	HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
	int     Capacity;               // ������� ��������� � ���������� ��������� 
	int     SecSnapshotInterval;    // ������������� ���������� � ���. 
	int     MaxKeyLength;           // ������������ ����� �����
	int     MaxPayloadLength;       // ������������ ����� ������
	char    FileName[512];          // ��� ����� 
	HANDLE  File;                   // File HANDLE != 0, ���� ���� ������
	HANDLE  FileMapping;            // Mapping File HANDLE != 0, ���� mapping ������  
	LPVOID  Addr;                   // Addr != NULL, ���� mapview ��������  
	char    LastErrorMessage[512];  // ��������� �� ��������� ������ ��� 0x00  
	time_t  lastsnaptime;			// ���� ���������� snap'a (time())  
	int CurrentElements;            //Added: the number of elements in a storage
};

extern "C" struct Element   // ������� 
{
	Element();
	Element(const void* key, int keylength);                                             // for Get
	Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
	Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
	const void* key;                 // �������� ����� 
	int             keylength;           // ������ �����
	const void* payload;             // ������ 
	int             payloadlength;       // ������ ������
};

extern "C" HTHANDLE* Create   //  ������� HT             
(
	int	  Capacity,					   // ������� ���������
	int   SecSnapshotInterval,		   // ������������� ���������� � ���.
	int   MaxKeyLength,                // ������������ ������ �����
	int   MaxPayloadLength,            // ������������ ������ ������
	const char  FileName[512]          // ��� ����� 
); 	// != NULL �������� ����������  


extern "C" HTHANDLE* Open     //  ������� HT             
(
	const char    FileName[512]         // ��� ����� 
); 	// != NULL �������� ����������  


extern "C" BOOL Snap         // ��������� Snapshot
(
	HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
);

extern "C" BOOL Close        // Snap � ������� HT  �  �������� HTHANDLE
(
	HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
);	//  == TRUE �������� ����������   

extern "C" BOOL Insert      // �������� ������� � ���������
(
	HTHANDLE* hthandle,            // ���������� HT
	const Element* element              // �������
);	//  == TRUE �������� ���������� 

extern "C" BOOL Delete      // ������� ������� � ���������
(
	HTHANDLE* hthandle,            // ���������� HT (����)
	const Element* element              // ������� 
);	//  == TRUE �������� ���������� 


extern "C" Element* Get     //  ������ ������� � ���������
(
	const HTHANDLE* hthandle,            // ���������� HT
	const Element* element              // ������� 
); 	//  != NULL �������� ���������� 


extern "C" BOOL Update     //  ������� ������� � ���������
(
	const HTHANDLE* hthandle,            // ���������� HT
	const Element* oldelement,          // ������ ������� (����, ������ �����)
	const void* newpayload,          // ����� ������  
	int             newpayloadlength     // ������ ����� ������
); 	//  != NULL �������� ���������� 


extern "C" void Print                               // ����������� ������� 
(
	const Element* element              // ������� 
);


extern "C" const char* CreateSnapshotFileName(HTHANDLE* handle); //to create a snapshot file name that suits the folowing format: Snapshot-dd-mm-yyyy.htsnap


