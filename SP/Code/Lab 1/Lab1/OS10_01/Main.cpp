#include "OS10_1.h"
#include <iostream>

using namespace std;


int main() {

	HT::HTHANDLE* ht = nullptr;
	try {
		ht = HT::Create(1000, 3, 10, 256, "C:\\Users\\Manmade\\Desktop\\5sem\\SP\\Code\\Lab 1\\HTSpace.ht");
		if (ht) {
			cout << "--Creation Successful" << endl;
		}
		else {
			cout << "--Creation Failed" << endl;
		}

		if (HT::Insert(ht, new HT::Element("TestKey",8,"TestPayload",12))) {
			std::cout << "--Insertion Successful" << std::endl;
		}
		else {
			std::cout << "--Insertion Failed" << std::endl;
		}

	}
	catch (char* msg) {
		cout << msg << endl;
		
	}


	return 0;

}