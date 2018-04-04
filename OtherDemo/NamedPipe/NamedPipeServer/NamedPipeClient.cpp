#include "stdafx.h"
#define BUFFER_MAX_SIZE 256
const char* pStrPipeName = "\\\\.\\pipe\\NamePipe_MoreWindows";
int main()
{
	printf("    命名管道 客户端\n");
	printf("---- by MoreWindows(http) ----\n\n");
	printf("按任意键开始连接命名管道\n");
	getch();//getchar??
	printf("开始等待命名管道\n");
	if(WaitNamedPipe(pStrPipeName,NMPWAIT_WAIT_FOREVER) == FALSE)
	{
		printf("错误！连接命名管道失败\n");
		return -1;
	}
	printf("打开命名管道\n");
	HANDLE hPipe = CreateFile(
		pStrPipeName,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	printf("向服务端发送数据\n");
	char szBuffer[BUFFER_MAX_SIZE];
	memset(szBuffer,0,BUFFER_MAX_SIZE);
	DWORD dwLen = 0;
	//向服务端发送数据
	sprintf(szBuffer,"进程%d说\"%s\"",GetCurrentProcessId())
	

}