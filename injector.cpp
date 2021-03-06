﻿// injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<Windows.h>
#include"detours.h"
#include <tlhelp32.h>
#include<iostream>
using namespace std;

DWORD MyGetProcessId(LPCTSTR ProcessName)
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt))
	{
		do
		{
			if (!lstrcmpi(pt.szExeFile, ProcessName)) 
			{
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);
	return 0;
}

BOOL InjectDynamicLibrary(DWORD processid, char *dllPath)
{
	HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processid);
	if (hTargetProcess)
	{

		LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryA");
		cout << "\nGot Proc addr kernel32";
		LPVOID LoadPath = VirtualAllocEx(hTargetProcess, 0, strlen(dllPath)+1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		cout << "\nload path done";

		WriteProcessMemory(hTargetProcess, (LPVOID)LoadPath, dllPath, strlen(dllPath)+1, NULL);
		cout << "\nWrite proc memory";

		HANDLE RemoteThread = CreateRemoteThread(hTargetProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibAddr, LoadPath, 0, 0);
		cout << "\nRemote thread created";

		WaitForSingleObject(RemoteThread, INFINITE);
		cout << "\nwait for single object";

		/*VirtualFreeEx(hTargetProcess, LoadPath, strlen(dllPath), MEM_RELEASE);
		cout << "\nvirtualfreeex";*/

		CloseHandle(RemoteThread);
		cout << "\nclose handle thread"; 

		CloseHandle(hTargetProcess);
		cout << "\nclose handle process ";

		return TRUE;
	}

	else
	return FALSE;
}

int main()
{
	char dllPath[] = "E:\\dirDLL.dll";
	char *pdllPath = dllPath;

	DWORD pid = MyGetProcessId(TEXT("HxD.exe"));
	wcout << "Process id : " << pid << endl;
	cout << "DLL path : " << dllPath << endl;

	bool b = InjectDynamicLibrary(pid, dllPath);
	if (b == FALSE)
	{
	cout << "FALSE\n";
	}

	system("PAUSE");
	return 0;

}

