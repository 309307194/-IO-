#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <process.h>

HANDLE hCicp = NULL;

unsigned int ThreadFunc(void* arg)
{
	getchar();
	OVERLAPPED ol = { 0 };
	ULONG_PTR key = 10;
	PostQueuedCompletionStatus(hCicp, 4, key, &ol);
	return 0;
}

int main()
{

	char username[255] = { 0 };
	HANDLE hFile = CreateFile(_T("1.txt"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		printf("Create File failed...\n");
		return 1;
	}

	//HANDLE hCicp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	hCicp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hCicp == NULL)
	{
		printf("Create IoCompletion Port failed...\n");
		return 1;
	}
	ULONG_PTR CK_READ = 0;
	CreateIoCompletionPort(hFile, hCicp, CK_READ, 0);

	unsigned int uiThreadID = 0;
	HANDLE hth = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)ThreadFunc, NULL, 0, &uiThreadID);

	OVERLAPPED ol = { 0 };
	BOOL ret = ReadFile(hFile, username, 255, NULL, &ol);
	if (ret)
	{
		return 1;
	}

	DWORD transferedByte = 0;
	void* lpContext = NULL;
	OVERLAPPED* pOl = NULL;
	while (GetQueuedCompletionStatus(hCicp, &transferedByte, (LPDWORD)&lpContext, &pOl, INFINITE))
	{
		if (lpContext != NULL && 10 == (unsigned long)lpContext)
		{
			printf("exit \n");
			break;
		}
		printf("%s", username);
	}

	CloseHandle(hth);
	CloseHandle(hFile);
	CloseHandle(hCicp);


	return 0;
}

