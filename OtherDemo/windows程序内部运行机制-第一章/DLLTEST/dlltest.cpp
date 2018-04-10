#include "stdafx.h"
using namespace std;

int main()
{
	CString str;
	str.Format("a + b =%d",add(1,5));
	cout<<str;
	getchar();
	return 0;
}