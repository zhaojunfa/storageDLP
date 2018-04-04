#include "stdafx.h"
#include <stdio.h>
#include <conio.h>

#define BUFFER_MAX_LEN 256

const char* pStrPipeName = "\\\\.\\pipe\\NamePipe_MoreWindows";

int main()
{
	printf("     命名管道服务器\n");
	printf("----by MoreWindows(http) ----\n\n");
	printf("创建命名管道并等待连接\n");
	HANDLE hPipe = CreateNamedPipe(
		pStrPipeName,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		NMPWAIT_WAIT_FOREVER,
		0);
	if(ConnectNamedPipe(hPipe,NULL) != NULL)//等待连接
	{
		printf("连接成功，开始接收数据\n");
		char szBuffer[BUFFER_MAX_LEN];
		DWORD dwLen = 0;

		//接收客户端发送的数据
		ReadFile(hPipe,szBuffer,BUFFER_MAX_LEN,&dwLen,NULL);
		printf("接收到数据长度为%d字节\n",dwLen);
		printf("具体数据内容如下:%s\n",szBuffer);

		//确认已经收到数据
		printf("向客户端发送已经收到数据标志\n");
		strcpy(szBuffer,"服务器已经收到");
		WriteFile(hPipe,szBuffer,strlen(szBuffer)+1,&dwLen,NULL);
	}

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	return 0;
}