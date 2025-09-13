#include <iostream>
#include "OS10_1.h"
#include <mutex>
#include <thread>
#include <cstring>
#include <Windows.h>

using namespace std;

namespace HT {

	std::mutex ht_mutex;

    HTHANDLE::HTHANDLE() :
        Capacity(0),
        SecSnapshotInterval(0),
        MaxKeyLength(0),
        MaxPayloadLength(0),
        File(NULL),
        FileMapping(NULL),
        Addr(NULL),
        lastsnaptime(0)
    {
        ZeroMemory(LastErrorMessage, sizeof(LastErrorMessage));
	}

	HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512])

		: Capacity(Capacity), SecSnapshotInterval(SecSnapshotInterval),

		MaxKeyLength(MaxKeyLength), MaxPayloadLength(MaxPayloadLength), lastsnaptime(0),
        File(NULL),FileMapping(NULL),Addr(NULL)
    {

        strcpy_s(this->FileName, 512, FileName);

        ZeroMemory(LastErrorMessage, sizeof(LastErrorMessage));

	}

	Element::Element() :
		key(nullptr),
		keylength(0),
		payload(nullptr),
		payloadlength(0) {}

	Element::Element(const void* key, int keylength): key(key),keylength(keylength){}

	Element::Element(const void* key, int keylength, const void* payload, int  payloadlength):
		key(key),keylength(keylength),payload(payload),payloadlength(payloadlength){}

	HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) {//should return NULL if smth went wrong
        HTHANDLE* ht = new HTHANDLE(Capacity,SecSnapshotInterval,MaxKeyLength,MaxPayloadLength,FileName);

        ht->File = CreateFileA(
            "HTStorage.ht",//file name
            GENERIC_READ | GENERIC_WRITE,//desired access
            FILE_SHARE_READ | FILE_SHARE_WRITE,//share mode(for other processes)
            NULL,//security attributes
            OPEN_ALWAYS,//creation disposition. here - opens existing or creates a new one if does not exist
            FILE_ATTRIBUTE_NORMAL,//file attributes. here - normal file with no specified attributes
            NULL//template file for handle. here no templates are being used
        );

        if (ht->File == INVALID_HANDLE_VALUE) {
            cout << "--File Creation Failed(Create)--" << endl;
            delete ht;
            return NULL;
        }
        else {
            cout << "--Creation Successful(Create)--" << endl;
        }

        ht->FileMapping = CreateFileMappingA(
            ht->File,//handle to a file for which a FileMapping is being created
            NULL,//security descriptor pointer. in that case NULL represents a default security descriptor
            PAGE_READWRITE,//protection for the mapping. now file is allowed to be read and written to
            0,//max mapping size
            0,//min mapping size(both max and min values are 0 so the max and min file size is decided by the actual file size)
            "SharedHTMapping"//named mapping. that means other processes can access the file(if NULL - they can not)
        );
        if (ht->FileMapping == NULL) {
            cout << "--File Mapping Creation Failed(Create)--" << endl;
            return NULL;
        }
        else {
            cout << "--File Mapping Successful(Create)--" << endl;
        }

        ht->Addr = MapViewOfFile(
            ht->FileMapping,//handle to the file mapping created earlier
            FILE_MAP_ALL_ACCESS,//access level option
            0,//file offset high - specifies the offset where the view starts. needed if we need to map a portion of a file
            0,//file offset low - specifies the offset where the view ends. needed if we need to map a portion of a file
            0//number of bytes to map. if 0 - the whole file mapping file will be mapped into RAM
        );

        if (ht->Addr == NULL) {
            cout << "--Failed to Map View Of File(Create)--" << endl;
            return NULL;
        }
        else {
            cout << "--MapViewOfFile successful(Create)--" << endl;
        }

        return ht;

	}

    //the only parameter difference here is file open mode in CreateFileA function. the rest is identical
    HTHANDLE* Open(const char FileName[512]) {
        HTHANDLE* ht = new HTHANDLE();
        ht->File = CreateFileA(
            FileName,
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,//this is the only difference in a parameters of CreateFileA function. here we hope that there is a file with FileName and try to open it
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (ht->File == INVALID_HANDLE_VALUE) {
            cout << "--File Creation Failed(Open)--" << endl;
            delete ht;
            return NULL;
        }
        else {
            cout << "--File Creation Successful(Open)--" << endl;
        }
        ht->FileMapping = CreateFileMappingA(
            ht->File,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            "SharedHTMapping"
        );
        if (ht->FileMapping == NULL) {
            cout << "--File Mapping Failed(Open)--" << endl;
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            cout << "--File Mapping Successful(Open)--" << endl;
        }

        ht->Addr = MapViewOfFile(
            ht->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0
        );
        if (ht->Addr == NULL) {
            cout << "--Map View Of File Failed(Open)--" << endl;
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            cout << "--Map View Of File Successful(Open)--" << endl;
        }
        return ht;
    }

    BOOL Snap(const HTHANDLE* hthandle) {
        if (!hthandle|| !hthandle->Addr) {
            cout << "--HThandle was invalid or Address was NULL(Snap)--" << endl;
            return FALSE;
        }

        const char SnapshotFileName[] = "Snapshot.htsnapshot";

        HANDLE htSnapshotFile = CreateFileA(
            SnapshotFileName,
            GENERIC_READ|GENERIC_WRITE,
            0,//sharing mode: here sharing is not alowed
            NULL,
            CREATE_ALWAYS,//create a new file or rewrite the content in already exisitng file. needed cause' we can have just one snapshot at a time
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (htSnapshotFile == INVALID_HANDLE_VALUE) {
            cout << "--Failed To Create a Snapshot File(Snap)--" << endl;
            return FALSE;
        }
        else {
            cout << "--Snapshot File Creation Successful(Snap)--" << endl;
        }

        SIZE_T dataSize = hthandle->Capacity * (hthandle->MaxKeyLength + hthandle->MaxPayloadLength);//determine the size of data for a snapshot

        DWORD bytesWritten;

        //need to clarify the meaning of the parametres and the way this function works
        BOOL writeResult = WriteFile(
            htSnapshotFile,
            hthandle->Addr,
            dataSize,
            &bytesWritten,
            NULL
        );

        if (!writeResult || bytesWritten != dataSize) {
            cout << "--Failed To Take a Snapshot(Snap)--" << endl;
            CloseHandle(htSnapshotFile);
            return FALSE;

        }
        else {
            cout << "--Snapshot Taken Successful(Snap)--" << endl;
        }

    }

    BOOL Insert(const HTHANDLE* hthandle, const Element* element) {
        std::lock_guard<std::mutex>lock(ht_mutex);

        if (!hthandle || !element || element->keylength > hthandle->MaxKeyLength || element->payloadlength > hthandle->MaxPayloadLength) {
            std::cout << "Failed to insert an element" << std::endl;
            return FALSE;
        }


    }
	
};