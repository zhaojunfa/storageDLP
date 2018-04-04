#include "stdafx.h"
#include <stdio.h>
#include <conio.h>

#define BUFFER_MAX_LEN 256

const char* pStrPipeName = "\\\\.\\pipe\\NamePipe_MoreWindows";

int main()
{
	printf("     �����ܵ�������\n");
	printf("----by MoreWindows(http) ----\n\n");
	printf("���������ܵ����ȴ�����\n");
	HANDLE hPipe = CreateNamedPipe(
		pStrPipeName,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		NMPWAIT_WAIT_FOREVER,
		0);
	if(ConnectNamedPipe(hPipe,NULL) != NULL)//�ȴ�����
	{
		printf("���ӳɹ�����ʼ��������\n");
		char szBuffer[BUFFER_MAX_LEN];
		DWORD dwLen = 0;

		//���տͻ��˷��͵�����
		ReadFile(hPipe,szBuffer,BUFFER_MAX_LEN,&dwLen,NULL);
		printf("���յ����ݳ���Ϊ%d�ֽ�\n",dwLen);
		printf("����������������:%s\n",szBuffer);

		//ȷ���Ѿ��յ�����
		printf("��ͻ��˷����Ѿ��յ����ݱ�־\n");
		strcpy(szBuffer,"�������Ѿ��յ�");
		WriteFile(hPipe,szBuffer,strlen(szBuffer)+1,&dwLen,NULL);
	}

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	return 0;
}