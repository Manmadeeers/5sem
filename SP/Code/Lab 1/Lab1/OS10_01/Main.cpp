#include "OS10_1.h"
#include <iostream>
#define CREATION_ENABLED
//#define OPENING_ENABLED
#define INSERTION_ENABLED
//#define DELETION_ENABLED
#define CLOSURE_ENABLED
//#define GET_ENABLED
//#define UPDATE_ENABLED
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


		cout << endl << "----------Creation Ended----------" << endl;

#endif // CREATION_ENABLED

		

#ifdef OPENING_ENABLED
		handle = HT::Open("Test.ht");
		if (handle == NULL) {
			cout << "--Failed to open existing HT_Storage--" << endl;
		}
		else {
			cout << "--Existing HT-Storage opened successfully--" << endl;
		}

		cout << endl << "----------Opening Ended----------" << endl;
#endif // OPENING_ENABLED


#ifdef INSERTION_ENABLED
		cout << endl << "----------Insertion Started----------" << endl << endl;
		HT::Insert(handle, new HT::Element("key", 3, "payload", 7));

		HT::Insert(handle, new HT::Element("key1", 4, "PAYLOAD1", 8));

		HT::Insert(handle, new HT::Element("key2", 4, "payload2", 8));
		HT::Insert(handle, new HT::Element("key3", 4, "payload3", 8));
		HT::Insert(handle, new HT::Element("key4", 4, "payload4", 8));
		HT::Insert(handle, new HT::Element("key5", 4, "payload5", 8));
		HT::Insert(handle, new HT::Element("key6", 4, "payload6", 8));
		HT::Insert(handle, new HT::Element("key7", 4, "payload7", 8));
		HT::Insert(handle, new HT::Element("key8", 4, "payload8", 8));
		HT::Insert(handle, new HT::Element("key9", 4, "payload9", 8));

		cout << endl << "----------Insertion Ended----------" << endl;
	
#endif // INSERTION_ENABLED

#ifdef DELETION_ENABLED
		HT::Delete(handle, new HT::Element("key", 3, "payload", 7));

#endif // DELETION_ENABLED
#ifdef GET_ENABLED
		cout << endl << "----------Get Started----------" << endl << endl;
		HT::Element* got_element = HT::Get(handle, new HT::Element("key7", 4, "payload7", 8));
		if (got_element != NULL) {
			cout << "--Get executed successful--" << endl;
			HT::Print(got_element);
		}
		else {
			cout << "--Get failed to execute--" << endl;
		}

		cout << endl << "----------Get Ended----------" << endl;
#endif // GET_ENABLED
#ifdef UPDATE_ENABLED	
		cout << endl << "----------Update Started----------" << endl << endl;
		if (HT::Update(handle, new HT::Element("key1", 4, "payload1", 8), "updPayload", 10)) {
			cout << "--Element updated successfully--" << endl;
		}
		else {
			cout << "--Element was not updated--" << endl;
		}

		cout << "----------Update Ended----------" << endl;
#endif // UPDATE_ENABLED



#ifdef CLOSURE_ENABLED
		if (HT::Close(handle)) {
			cout << "--Closed Successfully--" << endl;
		}
		else {
			cout << "--Failed to Close--" << endl;
		}
#endif // CLOSURE_ENABLED
		
	}
	catch (exception ex) {
		cout << ex.what() << endl;
		
	}


	return 0;

}