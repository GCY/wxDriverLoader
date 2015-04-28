#include <windows.h>
#include <tchar.h>

#include <cstdio>

int main(void)
{
	HANDLE h = CreateFile(_T("\\\\.\\TestDriver"),GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(h == INVALID_HANDLE_VALUE){
		printf("CreateFile Failed!\n");
		return -1;
	}
	else{
		printf("CreateFile Success!\n");
	}

	CloseHandle(h);

	getchar();
	return 0;
}