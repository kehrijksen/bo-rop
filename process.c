#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: ./process.exe <application_path>\n");
		return EXIT_FAILURE;
	}

	// LPSTR lpCommandLine = (char*)"calc.exe";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	printf("Starting: %s\n", argv[1]);

	if (!CreateProcess(argv[1],
		NULL, //lpCommandLine,
		NULL, 
		NULL, 
		FALSE, 
		0, 
		NULL, 
		NULL, 
		&si, 
		&pi)) 
	{
		printf("Could not create process!\n");
		return -69;
	}

	printf("PID: %lu\n", pi.dwProcessId);
	printf("Process handle: 0x%x\n", pi.hProcess);
	
	printf("TID: %lu\n", pi.dwThreadId);
	printf("Thread handle: 0x%x\n", pi.hThread);

	printf("\nWaiting for process to close...");
	WaitForSingleObject(pi.hProcess, INFINITE);
	printf("process closed!\n");
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return EXIT_SUCCESS;
}
