#include "OS10_1.h"
#include <iostream>
#define VAULT_PATH "C:\\Users\\Manmade\\Desktop\\5sem\\SP\\Code\\Lab 1\\HTSpace.ht"
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

		//if (HT::Open("C:\\Users\\Manmade\\Desktop\\5sem\\SP\\Code\\Lab 1\\HTSpace.ht")) {
		//	std::cout << "Opened Successfully at " << VAULT_PATH << std::endl;
		//}
		//else {
		//	std::cout << "Failed to open" << std::endl;
		//}
	}
	catch (char* msg) {
		cout << msg << endl;
		
	}


	return 0;

}