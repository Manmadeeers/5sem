#pragma once
#include "pch.h"
#include <Windows.h>


namespace HT {

	struct HTHANDLE    // ���� ���������� HT
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

	struct Element   // ������� 
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


	extern "C" {
		__declspec(dllexport)	HTHANDLE* Create   //  ������� HT             
		(
			int	  Capacity,					   // ������� ���������
			int   SecSnapshotInterval,		   // ������������� ���������� � ���.
			int   MaxKeyLength,                // ������������ ������ �����
			int   MaxPayloadLength,            // ������������ ������ ������
			const char  FileName[512]          // ��� ����� 
		); 	// != NULL �������� ����������  

		__declspec(dllexport) HTHANDLE* Open     //  ������� HT             
		(
			const char    FileName[512]         // ��� ����� 
		); 	// != NULL �������� ����������  

		__declspec(dllexport) BOOL Snap         // ��������� Snapshot
		(
			HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
		);

		__declspec(dllexport) BOOL Close        // Snap � ������� HT  �  �������� HTHANDLE
		(
			HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
		);	//  == TRUE �������� ����������   

		__declspec(dllexport) BOOL Insert(HTHANDLE* hthandle, const Element* element);

		__declspec(dllexport) BOOL Delete      // ������� ������� � ���������
		(
			HTHANDLE* hthandle,            // ���������� HT (����)
			const Element* element              // ������� 
		);	//  == TRUE �������� ���������� 

		__declspec(dllexport) Element* Get     //  ������ ������� � ���������
		(
			const HTHANDLE* hthandle,            // ���������� HT
			const Element* element              // ������� 
		); 	//  != NULL �������� ���������� 

		__declspec(dllexport) BOOL Update     //  ������� ������� � ���������
		(
			const HTHANDLE* hthandle,            // ���������� HT
			const Element* oldelement,          // ������ ������� (����, ������ �����)
			const void* newpayload,          // ����� ������  
			int             newpayloadlength     // ������ ����� ������
		); 	//  != NULL �������� ���������� 

		__declspec(dllexport) void Print                               // ����������� ������� 
		(
			const Element* element              // ������� 
		);

		__declspec(dllexport) const char* CreateSnapshotFileName(HTHANDLE* handle);

	}


};
