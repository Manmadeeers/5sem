#include <iostream>
#include "OS10_1.h"
using namespace std;

//HT::HTHANDLE* Open(const char FileName[512]) {
//
//}


HT::HTHANDLE* Create(int	  Capacity,					
	int   SecSnapshotInterval,		  
	int   MaxKeyLength,                
	int   MaxPayloadLength,           
	const char  FileName[512]) 
{
    HT::HTHANDLE* hthandle = new HT::HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
	return hthandle;

}