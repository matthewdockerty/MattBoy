#include <Windows.h>
#include <iostream>
#include <thread>

#include "GUIWindow.h"
#include "Menus.h"

std::shared_ptr<bool> quit;

void CreateConsoleWindow()
{
	AllocConsole();
	FILE* new_stdout;
	freopen_s(&new_stdout, "CONIN$", "r", stdout);
	freopen_s(&new_stdout, "CONOUT$", "w", stdout);
	freopen_s(&new_stdout, "CONOUT$", "w", stderr);
}

void CleanUp()
{
	FreeConsole();
}



void MainWindowMenuCallback(UINT_PTR itemId)
{
	switch (itemId)
	{
	case Menus::IDM_MAIN_FILE_QUIT:
		
		*quit = true;
		break;
	default:
		break;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CreateConsoleWindow();

	quit = std::make_shared<bool>(false);

	GUIWindow mainWindow("MattBoy Classic", 400, 400, NULL, Menus::CreateMainWindowMenus(), MainWindowMenuCallback, hInstance, true, quit);
	GUIWindow window2("Tile Viewer", 400, 400, mainWindow.GetHwnd(), NULL, nullptr, hInstance, false, quit);

	while (!*quit)
	{
		mainWindow.MessageProcess();
	}

	CleanUp();
}
