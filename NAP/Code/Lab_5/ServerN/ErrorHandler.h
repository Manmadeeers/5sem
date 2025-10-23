#pragma once
#include <iostream>
#include <winsock2.h>
#include <string>
#include <cstring>
using namespace std;

string SetErrorMsgText(string text, int code);
string GetErrorMsgText(int code);