#include <Windows.h>
#include <iostream>
#include <memory>

//#include "MattBoy/Gameboy.h"

#include "GUIWindow.h"
#include "Menus.h"


std::shared_ptr<bool> quit;
std::unique_ptr<GUIWindow> mainWindow, tilesWindow;
//std::unique_ptr<mattboy::Gameboy> gameboy;

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
	UINT state;

	switch (itemId)
	{

	case Menus::IDM_MAIN_FILE_LOAD_ROM: break;
	case Menus::IDM_MAIN_FILE_SAVE_STATE: break;
	case Menus::IDM_MAIN_FILE_LOAD_STATE: break;
	case Menus::IDM_MAIN_FILE_QUIT: *quit = true; break;

	case Menus::IDM_MAIN_EMULATION_PAUSE: break;
	case Menus::IDM_MAIN_EMULATION_RESET: break;
	case Menus::IDM_MAIN_EMULATION_SPEED_1: break;
	case Menus::IDM_MAIN_EMULATION_SPEED_2: break;
	case Menus::IDM_MAIN_EMULATION_SPEED_5: break;
	case Menus::IDM_MAIN_EMULATION_SPEED_10: break;

	case Menus::IDM_MAIN_VIEW_WINDOW_SIZE_100: break;
	case Menus::IDM_MAIN_VIEW_WINDOW_SIZE_200: break;
	case Menus::IDM_MAIN_VIEW_WINDOW_SIZE_300: break;
	case Menus::IDM_MAIN_VIEW_WINDOW_SIZE_400: break;
	case Menus::IDM_MAIN_VIEW_WINDOW_SIZE_500: break;
	case Menus::IDM_MAIN_VIEW_TILES: 
		state = GetMenuState(Menus::viewMenu, Menus::IDM_MAIN_VIEW_TILES, MF_BYCOMMAND);
		tilesWindow->SetVisible(state != MF_CHECKED);
		CheckMenuItem(Menus::viewMenu, Menus::IDM_MAIN_VIEW_TILES, state == MF_CHECKED ? MF_UNCHECKED : MF_CHECKED);
		break;
	case Menus::IDM_MAIN_VIEW_DEBUG: break;
	case Menus::IDM_MAIN_VIEW_COLOR_CLASSIC: break;
	case Menus::IDM_MAIN_VIEW_COLOR_GRAYSCALE: break;

	default:
		break;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CreateConsoleWindow();

	quit = std::make_shared<bool>(false);
	
	mainWindow = std::make_unique<GUIWindow>("MattBoy Classic", 400, 400, (HWND)NULL, Menus::CreateMainWindowMenus(), MainWindowMenuCallback, hInstance, true, quit);
	tilesWindow = std::make_unique<GUIWindow>("Tile Viewer", 300, 200, mainWindow->GetHwnd(), (HMENU)NULL, nullptr, hInstance, false, quit);

	//gameboy = std::make_unique<mattboy::Gameboy>();

	while (!*quit)
	{
		mainWindow->MessageProcess();
	}

	CleanUp();
}
