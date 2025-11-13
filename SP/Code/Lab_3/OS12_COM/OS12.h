#pragma once
#define OS12HANDLE void*
namespace OS12
{
	OS12HANDLE Init();                             
	//   if CoCreateInstance(... IID_Unknown)!= successful --> throw (int)HRESULT  
	namespace Adder
	{
		double Add(OS12HANDLE h, double x, double y);        // return x+y
		//  if QueryInterface(IID_IAdder) != succesfull -->  throw (int)HRESULT     
		double Sub(OS12HANDLE h, double x, double y);        // return x-y
		//  if QueryInterface(IID_IAdder) != successful -->  throw (int)HRESULT
	}
	namespace Multiplier
	{
		double Mul(OS12HANDLE h, double x, double y);        // return x*y
		//  if QueryInterface(IID_IMultiplier) != successful -->  throw (int)HRESULT 
		double Div(OS12HANDLE h, double x, double y);        // return x/y
		//  if QueryInterface(IID_IMultiplier) != successful -->  throw (int)HRESULT 
	}
	void Dispose(OS12HANDLE h);                        
}