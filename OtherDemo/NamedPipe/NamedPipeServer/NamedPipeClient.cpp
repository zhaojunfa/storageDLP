#include "stdafx.h"
#define BUFFER_MAX_SIZE 256
const char* pStrPipeName = "\\\\.\\pipe\\NamePipe_MoreWindows";
int main()
{
	printf("    �����ܵ� �ͻ���\n");
	printf("---- by MoreWindows(http) ----\n\n");
	printf("���������ʼ���������ܵ�\n");
	getch();//getchar??
	printf("��ʼ�ȴ������ܵ�\n");
	if(WaitNamedPipe(pStrPipeName,NMPWAIT_WAIT_FOREVER) == FALSE)
	{
		printf("�������������ܵ�ʧ��\n");
		return -1;
	}
	printf("�������ܵ�\n");
	HANDLE hPipe = CreateFile(
		pStrPipeName,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	printf("�����˷�������\n");
	char szBuffer[BUFFER_MAX_SIZE];
	memset(szBuffer,0,BUFFER_MAX_SIZE);
	DWORD dwLen = 0;
	//�����˷�������
	sprintf(szBuffer,"����%d˵\"%s\"",GetCurrentProcessId())
	

}