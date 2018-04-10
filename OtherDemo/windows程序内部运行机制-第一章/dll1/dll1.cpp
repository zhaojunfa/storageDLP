#include "stdafx.h"

_declspec(dllexport) int add(int a,int b)
{
	return a + b;

}
_declspec(dllexport) int subtract(int a,int b)
{
	return a - b;
}

typedef int(* ADDPROC)(int a,int b);
ADDPROC Add = (ADDPROC)GetProcAddress(LoadLibrary("dll1.dll"),"add");