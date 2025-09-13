#include "OS10_1.h"
#include <iostream>
#define CREATION_ENABLED
//#define OPENING_ENABLED
#define INSERTION_ENABLED
using namespace std;


int main() {
	HT::HTHANDLE* handle = nullptr;
	try {


#ifdef CREATION_ENABLED
		handle = HT::Create(1000, 3, 10, 256, "Test.ht");

		if (handle == NULL) {
			cout << "--Failed To Create Or Open An HT-Storage--" << endl;
		}
		else {
			cout << "--Storage Created Successfully--" << endl;
		}

#endif // CREATION_ENABLED

		

#ifdef OPENING_ENABLED
		handle = HT::Open("Test.ht");
		if (handle == NULL) {
			cout << "--Failed to open existing HT_Storage--" << endl;
		}
		else {
			cout << "--Existing HT-Storage opened successfully--" << endl;
		}
#endif // OPENING_ENABLED


#ifdef INSERTION_ENABLED

#endif // INSERTION_ENABLED

		
		
	}
	catch (exception ex) {
		cout << ex.what() << endl;
		
	}


	return 0;

}