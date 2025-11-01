#include <iostream>
#include <string>
#include "OS_11DLL.h"


int main(int argc, char* argv[]) {

	HT::HTHANDLE* storage = HT::Create(100,20,10,10,"st.ht");
	if (!storage) {
		std::cerr << "Failed to create" << std::endl;
	}

	srand((unsigned)time(NULL));
	int rand_key = rand() % 50;
	std::string s = "key"+std::to_string(rand_key);
	const char* key = s.c_str();
	std::cout << "Constructed key: " << key << std::endl;

	HT::Element* element = new HT::Element(key, (int)strlen(key), "pay", 3);
	if (!HT::Insert(storage, element)) {
		std::cerr << "Failed to insert" << std::endl;
	}

	HT::Element* got = HT::Get(storage, new HT::Element(key, (int)strlen(key)));
	if (!got) {
		std::cerr << "Failed to get an element" << std::endl;
	}
	if (!got->key||!got->payload) {
		std::cerr << "Contents corrupted" << std::endl;
	}

	HT::Print(got);

	system("pause");
	return 0;
}