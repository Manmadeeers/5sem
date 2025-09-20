#include <iostream>
#include "OS_10_2.h"
#include <cassert>
using namespace std;

void testCreate() {

    HT::HTHANDLE* handle = HT::Create(1000, 3, 10, 256, "Test.ht");

    assert(handle != NULL);

    cout << "Create Test Passed" << endl;

    HT::Close(handle);

}

void testOpen() {

    HT::HTHANDLE* handle = HT::Open("Test.ht");

    assert(handle != NULL);

    cout << "Open Test Passed" << endl;

    HT::Close(handle);
}


void testInsert() {

    HT::HTHANDLE* handle = HT::Create(1000, 3, 10, 256, "Test.ht");

    HT::Insert(handle, new HT::Element("key1", 4, "payload1", 8));

    HT::Insert(handle, new HT::Element("key2", 4, "payload2", 8));

    assert(handle->CurrentElements == 2);

    cout << "Insert Test Passed" << endl;

    HT::Close(handle);
}


void testDelete() {

    HT::HTHANDLE* handle = HT::Create(1000, 3, 10, 256, "Test.ht");

    HT::Insert(handle, new HT::Element("key1", 4, "payload1", 8));

    HT::Delete(handle, new HT::Element("key1", 4, "payload1", 8));

    assert(handle->CurrentElements == 0);

    cout << "Delete Test Passed" << endl;

    HT::Close(handle);

}

void testGet() {

    HT::HTHANDLE* handle = HT::Create(1000, 3, 10, 256, "Test.ht");

    HT::Insert(handle, new HT::Element("key1", 4, "payload1", 8));

    HT::Element* retrieved = HT::Get(handle, new HT::Element("key1", 4, "payload1", 8));

    assert(retrieved != NULL);

    assert(memcmp(retrieved->key, "key1", 4) == 0);

    delete retrieved;

    cout << "Get Test Passed" << endl;

    HT::Close(handle);

}

void testUpdate() {

    HT::HTHANDLE* handle = HT::Create(1000, 3, 10, 256, "Test.ht");

    HT::Insert(handle, new HT::Element("key1", 4, "payload1", 8));

    bool success = HT::Update(handle, new HT::Element("key1", 4, "payload1", 8), "updatedPayload", 13);

    assert(success);

    cout << "Update Test Passed" << endl;

    HT::Close(handle);

}

int main() {

    testCreate();

    testOpen();

    testInsert();

    testDelete();

    testGet();

    testUpdate();
	return 0;
}