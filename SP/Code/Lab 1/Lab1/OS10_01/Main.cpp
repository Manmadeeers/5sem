#include "OS10_1.h"
#include <iostream>

using namespace std;


int main() {

	HT::HTHANDLE* ht = nullptr;
	try {
		ht = HT::Create(1000, 3, 10, 256, "C:\\Users\\Manmade\\Desktop\\5sem\\SP\\Code\\Lab 1\\HTSpace.ht");
		if (ht) {
			cout << "--Creation Succesfull" << endl;
		}
		else {
			cout << "--Creation Failed" << endl;
		}


		if (HT::Close(ht)) {
			cout << "--Closure Succesfull" << endl;
		}
		else {
			cout << "--Closure Failed" << endl;
		}


	}
	catch (char* msg) {
		cout << msg << endl;
		
	}


	return 0;

}