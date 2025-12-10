#include <iostream>
#include <cassert>
#include "OS_10_2.h"

using namespace std;

HT::Element* makeElem(const char* key, int keylen, const char* payload, int payloadlen) {
    return new HT::Element(key, keylen, payload, payloadlen);
}

void testCreateOpenLifecycle() {
    HT::HTHANDLE* h1 = HT::Create(1000, 3, 10, 256, "Test.ht");
    assert(h1 != NULL);
    cout << "Lifecycle: Create passed" << endl;
    HT::Close(h1);

    HT::HTHANDLE* h2 = HT::Open("Test.ht");
    assert(h2 != NULL);
    cout << "Lifecycle: Open passed" << endl;
    HT::Close(h2);

    HT::HTHANDLE* h3 = HT::Create(1000, 3, 10, 256, "Test.ht");
    assert(h3 != NULL);
    cout << "Lifecycle: Re-create passed" << endl;
    HT::Close(h3);
}

void testInsertEdgeCases() {
    HT::HTHANDLE* h = HT::Create(2, 3, 10, 256, "Test2.ht");
    assert(h != NULL);

    HT::Insert(h, new HT::Element("k1", 2, "p1", 2));
    HT::Insert(h, new HT::Element("k1", 2, "p2", 2));

    assert(h->CurrentElements == 2);

    bool ok = HT::Insert(h, new HT::Element("k2", 2, "pp", 2));
    if (ok) {
        assert(false && "Insert should have failed due to capacity");
    }

    HT::Close(h);
}

void testGetUpdateDeleteFlow() {
    HT::HTHANDLE* h = HT::Create(3, 3, 6, 16, "Test3.ht");
    assert(h != NULL);

    HT::Insert(h, new HT::Element("key", 3, "val1", 4));
    HT::Insert(h, new HT::Element("K2", 2, "val2", 4));

    HT::Element* e = HT::Get(h, new HT::Element("key", 3, "val1", 4));
    assert(e != NULL);
    assert(memcmp(e->key, "key", 3) == 0);
    delete e;

    HT::Element* absent = HT::Get(h, new HT::Element("abs", 3, "zzz", 3));
    assert(absent == NULL);

    bool up = HT::Update(h, new HT::Element("key", 3, "val1", 4), "newP", 4);
    assert(up);

    bool up2 = HT::Update(h, new HT::Element("nope", 4, "x", 1), "doesnt", 6);
    cout << "Update non-existent result: " << (up2 ? "TRUE" : "FALSE") << endl;

    bool del = HT::Delete(h, new HT::Element("key", 3, "", 0));
    assert(del);

    bool del2 = HT::Delete(h, new HT::Element("key", 3, "", 0));
    cout << "Second delete result (should be FALSE): " << (del2 ? "TRUE" : "FALSE") << endl;

    HT::Close(h);
}

void testOpenPersistence() {
    HT::HTHANDLE* h = HT::Create(5, 3, 10, 256, "Persist.ht");
    HT::Insert(h, new HT::Element("alpha", 5, "payload", 7));

    HT::Close(h);

    HT::HTHANDLE* h2 = HT::Open("Persist.ht");
    assert(h2 != NULL);

    HT::Element* e = HT::Get(h2, new HT::Element("alpha", 5, "payload", 7));
    assert(e != NULL);
    delete e;

    HT::Close(h2);
}

void testGetKeyOnlyBehavior() {
    HT::HTHANDLE* h = HT::Create(3, 3, 10, 256, "Test4.ht");
    HT::Insert(h, new HT::Element("abc", 3, "zzz", 3));

    HT::Element* g = HT::Get(h, new HT::Element("abc", 3, "", 0));
    assert(g != NULL);
    delete g;

    HT::Close(h);
}

int main() {
    testCreateOpenLifecycle();
    testInsertEdgeCases();
    testGetUpdateDeleteFlow();
    testOpenPersistence();
    testGetKeyOnlyBehavior();

    cout << "All test suites completed." << endl;
    return 0;
}