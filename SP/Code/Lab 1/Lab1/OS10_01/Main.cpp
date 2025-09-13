#include "OS10_1.h"
#include <iostream>
using namespace std;


int main() {
	HT::HTHANDLE* handle = nullptr;
	try {
		
		handle = HT::Create(1000, 3, 10, 256, "Test.ht");

		if (handle == NULL) {
			cout << "--Failed To Create Or Open An HT-Storage--" << endl;
		}
		else {
			cout << "--Storage Created Successfully--" << endl;
		}
		
	}
	catch (exception ex) {
		cout << ex.what() << endl;
		
	}


	return 0;

}