/*
Hooking_Main.exe
Made by Tigre000
*/

#include "stdafx.h"
typedef void(*HOOK)(void);
typedef void(*UNHOOK)(void);
void foundProcess(const wchar_t *);

int main()
{
	HMODULE hDll = LoadLibraryA("..\\..\\..\\Hooking_DLL\\x64\\Release\\Hooking_DLL.dll");	// 만들어둔 DLL를 로드시키고
	HOOK Hooking = (HOOK)GetProcAddress(hDll, "Hooking");									// DLL의 후킹함수를 받아오고
	UNHOOK UnHooking = (UNHOOK)GetProcAddress(hDll, "UnHooking");							// 언후킹함수도 받아옵니다
	foundProcess(L"chrome.exe");															// foundProcess 함수는 프로세스가
	foundProcess(L"notepad.exe");															// 실행 중인지 확인하는 함수입니다
	Hooking();																				// 받아온 후킹함수 실행!!
	while (_getch() != 113);																// 메인프로그램의 콘솔에서 q가 입력되면
	UnHooking();																			// 언후킹함수 실행!!
	while (_getch() != 114);																// 하지만 한번만 실행하면 아쉽죠 r이 입력되면
	Hooking();																				// 다시 후킹!!
	while (_getch() != 113);																// q가 입력되면
	UnHooking();																			// 진짜 마지막으로 언후킹!!
	system("pause");																		// 잠시 쉬었다가
	return 0;																				// 메인프로그램은 끝이 납니다
}

void foundProcess(const wchar_t * cmpstr)													// 이 함수는 지금은 없어진(OTIST와 합쳐진) 안티치트연구회에서 
{																							// 치트를 공부할 때 사용했는 함수입니다
	BOOL ProcFound;																			// 이 함수에 관해서는 주석을 생략하겠습니다
	HANDLE Snap;																			// OTIST에 들어오시면 공부하실 수 있어요!!
	while (1)																				// ex)사실 이 함수는 구지 필요 없지만 그냥 넣었습니다 ㅎㅎ
	{																						// ex2) OTIST = 오픈시큐어랩 청소년 보안 연구회
		Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Snap)
		{
			PROCESSENTRY32 PE32;
			PE32.dwSize = sizeof(PROCESSENTRY32);
			ProcFound = Process32First(Snap, &PE32);
			while (ProcFound)
			{
				if (wcscmp(PE32.szExeFile, cmpstr) == 0)
				{
					printf("%ls %d\n\n", PE32.szExeFile, PE32.th32ProcessID);
					CloseHandle(Snap);
					return;
				}
				ProcFound = Process32Next(Snap, &PE32);
			}
		}
		printf("%ls is Not Found\n\n", cmpstr);
		Sleep(1000);
	}
}