#include "stdafx.h"

#define dll2_api _declspec(dllexport)

int add(int a,int b)
{
	return a + b;
}

int subtract(int a,int b)
{
	return a - b;
}