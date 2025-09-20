#include <iostream>
#include "OS_10_2.h"
using namespace std;

void testCreate() {

    HTHANDLE* handle = HT::Create(1000, 3, 10, 256, "Test.ht");

    assert(handle != NULL);

    cout << "Create Test Passed" << endl;

    HT::Close(handle);

}

int main() {

	return 0;
}