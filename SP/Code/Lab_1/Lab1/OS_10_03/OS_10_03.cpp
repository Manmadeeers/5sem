#include <iostream>
#include "OS_10_2.h"
using namespace std;

//#define CREATION_ENABLED
#define OPENING_ENABLED
#define INSERTION_ENABLED
#define DELETION_ENABLED
#define CLOSURE_ENABLED
#define GET_ENABLED
#define UPDATE_ENABLED

int main() {
	HT::HTHANDLE* handle1 = nullptr;
	HT::HTHANDLE* handle2 = nullptr;
	try {

#ifdef CREATION_ENABLED
		handle1 = HT::Create(20, 3, 10, 256, "TestMutual.ht");

		if (handle1 == NULL) {
			cout << "--Failed To Create Or Open An HT-Storage--" << endl;
		}
		else {
			cout << "--Storage Created Successfully--" << endl;
		}
#endif // CREATION_ENABLED

#ifdef OPENING_ENABLED

		handle1 = HT::Open("TestMutual.ht");
		handle2 = HT::Open("TestMutual.ht");

		if (handle1 == NULL) {

			cout << "--Failed to open existing HT_Storage1--" << endl;

		}
		else {

			cout << "--Existing HT-Storage 1 opened successfully--" << endl;

		}

		if (handle2 == NULL) {
			cout << "--Failed to open existing HT_Storage2--" << endl;
		}
		else {
			cout << "--Existing HT_Storage2 opened successfully" << endl;
		}


#endif // OPENING_ENABLED

#ifdef INSERTION_ENABLED

		cout << endl << "----------Insertion Started----------" << endl << endl;


		cout << "--Inserting to the first openning" << endl;

		HT::Insert(handle1, new HT::Element("key1", 4, "payload1", 8));

		HT::Insert(handle1, new HT::Element("key2", 4, "payload2", 8));
		cout << "--First ended" << endl;


		cout << endl << "--Inserting to the second opening" << endl;
		HT::Insert(handle2, new HT::Element("keyA", 4, "payloadA", 8));

		HT::Insert(handle2, new HT::Element("keyB", 4, "payloadB", 8));
		cout << "--Second ended" << endl;

		cout << endl << "----------Insertion Ended----------" << endl;

#endif // INSERTION_ENABLED

#ifdef DELETION_ENABLED

		cout << "Deleting from the first opening" << endl;
		HT::Delete(handle1, new HT::Element("key1", 4));

		cout << "Deleting from the second opening" << endl;
		HT::Delete(handle2, new HT::Element("keyA", 4));

#endif // DELETION_ENABLED

#ifdef GET_ENABLED

		cout << endl << "----------Get Started----------" << endl << endl;


		HT::Element* got_element1 = HT::Get(handle1, new HT::Element("key2", 4));

		if (got_element1 != NULL) {

			cout << "--Get from Storage 1 executed successfully--" << endl;

			HT::Print(got_element1);

			delete got_element1;

		}
		else {

			cout << "--Get from Storage 1 failed to execute--" << endl;

		}

		HT::Element* got_element2 = HT::Get(handle2, new HT::Element("keyB", 4));

		if (got_element2 != NULL) {

			cout << "--Get from Storage 2 executed successfully--" << endl;



			HT::Print(got_element2);

			delete got_element2;

		}
		else {

			cout << "--Get from Storage 2 failed to execute--" << endl;

		}

		cout << endl << "----------Get Ended----------" << endl;


#endif // GET_ENABLED

#ifdef UPDATE_ENABLED
		cout << endl << "----------Update Started----------" << endl << endl;

		if (HT::Update(handle1, new HT::Element("key2", 4), "updatedPayload", 14)) {

			cout << "--Element in Storage 1 updated successfully--" << endl;

		}
		else {

			cout << "--Element in Storage 1 was not updated--" << endl;

		}

		if (HT::Update(handle2, new HT::Element("keyB", 4), "updatedPayloadForB", 19)) {

			cout << "--Element in Storage 2 updated successfully--" << endl;

		}
		else {

			cout << "--Element in Storage 2 was not updated--" << endl;

		}
		cout << "----------Update Ended----------" << endl;

#endif // UPDATE_ENABLED


#ifdef CLOSURE_ENABLED

		if (HT::Close(handle1)) {

			cout << "--Closed Storage 1 Successfully--" << endl;

		}
		else {

			cout << "--Failed to Close Storage 1--" << endl;

		}

		if (HT::Close(handle2)) {

			cout << "--Closed Storage 1 Successfully--" << endl;

		}
		else {

			cout << "--Failed to Close Storage 1--" << endl;

		}



#endif // CLOSURE_ENABLED


	}
	catch (char* msg) {
		cerr << "Error: " << msg << endl;
	}


	return 0;
}