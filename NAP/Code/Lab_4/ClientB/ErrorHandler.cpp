#include "ErrorHandler.h"


string SetErrorMsgText(string text, int code) {
	char code_chr = static_cast<char>(code);
	return text + GetErrorMsgText(code) + "(" + code_chr + ")";
}

string GetErrorMsgText(int code) {

	string message;

	switch (code) {
	case WSAEINTR:
		message = "Function was stopped in the working process";
		break;
	case WSAEACCES:
		message = "Access denied";
		break;
	case WSAEFAULT:
		message = "Wring Address";
		break;
	case WSAEINVAL:
		message = "Argument Error";
		break;
	case WSAEMFILE:
		message = "Too many files opened";
		break;
	case WSAEWOULDBLOCK:
		message = "The resource is temporary unavailable";
		break;
	case WSAEINPROGRESS:
		message = "Operation is in development process";
		break;
	case WSAEALREADY:
		message = "Operation is already in execution process";
		break;
	case WSAENOTSOCK:
		message = "Socket set incorrectly";
		break;
	case WSAEDESTADDRREQ:
		message = "Address required";
		break;
	case WSAEMSGSIZE:
		message = "The message is too long";
		break;
	case WSAEPROTOTYPE:
		message = "Wring protocol type for a socket";
		break;
	case WSAENOPROTOOPT:
		message = "Error in a protocol option";
		break;
	case WSAEPROTONOSUPPORT:
		message = "Protocol is not supported";
		break;
	case WSAESOCKTNOSUPPORT:
		message = "Socket type is not supported";
		break;
	case WSAEOPNOTSUPP:
		message = "Operation is not supported";
		break;
	case WSAEPFNOSUPPORT:
		message = "Protocol type is not supported";
		break;
	case WSAEAFNOSUPPORT:
		message = "Address type is not supported";
		break;
	case WSAEADDRINUSE:
		message = "Address is already in use";
		break;
	case WSAEADDRNOTAVAIL:
		message = "Requested address could not be used";
		break;
	case WSAENETDOWN:
		message = "Network disconnected";
		break;
	case WSAENETUNREACH:
		message = "Could not reach the network";
		break;
	case WSAENETRESET:
		message = "The network shot down the connection";
		break;
	case WSAECONNABORTED:
		message = "Program denial of network";
		break;
	case WSAECONNRESET:
		message = "Connection restored";
		break;
	case WSAENOBUFS:
		message = "Not enough memory for buffers";
		break;
	case WSAEISCONN:
		message = "Socket is already connected";
		break;
	case WSAENOTCONN:
		message = "Socket disconnected";
		break;
	case WSAESHUTDOWN:
		message = "Could not execute send(): socket shut down";
		break;
	case WSAETIMEDOUT:
		message = "Connection timed out";
		break;
	case WSAECONNREFUSED:
		message = "Connection refused";
		break;
	case WSAEHOSTDOWN:
		message = "Host shut down";
		break;
	case WSAEHOSTUNREACH:
		message = "No route for host";
		break;
	case WSAEPROCLIM:
		message = "Too many processes";
		break;
	case WSASYSNOTREADY:
		message = "Network unreachable";
		break;
	case WSAVERNOTSUPPORTED:
		message = "This version is not supported";
		break;
	case WSANOTINITIALISED:
		message = "WS2_32.DLL was not initialized";
		break;
	case WSAEDISCON:
		message = "Shut down is in process";
		break;
	case WSATYPE_NOT_FOUND:
		message = "Type not found";
		break;
	case WSAHOST_NOT_FOUND:
		message = "Host not found";
		break;
	case WSATRY_AGAIN:
		message = "Unautharized host not found";
		break;
	case WSANO_RECOVERY:
		message = "Undefined error";
		break;
	case WSANO_DATA:
		message = "No record by a requiered type";
		break;
	case WSA_INVALID_HANDLE:
		message = "Descriptor came in with an error";
		break;
	case WSA_INVALID_PARAMETER:
		message = "One or more parameters came in with an error";
		break;
	case WSA_IO_INCOMPLETE:
		message = "I/O object is not is a signal state";
		break;
	case WSA_IO_PENDING:
		message = "Operation is to be completed lately";
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		message = "Not enough memory";
		break;
	case WSA_OPERATION_ABORTED:
		message = "Operation aborted";
		break;
	case WSAEINVALIDPROCTABLE:
		message = "Invalid service";
		break;
	case WSAEINVALIDPROVIDER:
		message = "Invalid service version";
		break;
	case WSAEPROVIDERFAILEDINIT:
		message = "Failed to initialize a service";
		break;
	case WSASYSCALLFAILURE:
		message = "System call was aborted";
		break;

	}


	return message;
}