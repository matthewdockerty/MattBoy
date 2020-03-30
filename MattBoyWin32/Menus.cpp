#include "Menus.h"

const int
Menus::IDM_MAIN_FILE_LOAD_ROM,
Menus::IDM_MAIN_FILE_SAVE_STATE,
Menus::IDM_MAIN_FILE_LOAD_STATE,
Menus::IDM_MAIN_FILE_QUIT,

Menus::IDM_MAIN_EMULATION_PAUSE,
Menus::IDM_MAIN_EMULATION_RESET,
Menus::IDM_MAIN_EMULATION_SPEED_1,
Menus::IDM_MAIN_EMULATION_SPEED_2,
Menus::IDM_MAIN_EMULATION_SPEED_5,
Menus::IDM_MAIN_EMULATION_SPEED_10,

Menus::IDM_MAIN_VIEW_WINDOW_SIZE_100,
Menus::IDM_MAIN_VIEW_WINDOW_SIZE_200,
Menus::IDM_MAIN_VIEW_WINDOW_SIZE_300,
Menus::IDM_MAIN_VIEW_WINDOW_SIZE_400,
Menus::IDM_MAIN_VIEW_WINDOW_SIZE_500,
Menus::IDM_MAIN_VIEW_TILES,
Menus::IDM_MAIN_VIEW_DEBUG,
Menus::IDM_MAIN_VIEW_COLOR_CLASSIC,
Menus::IDM_MAIN_VIEW_COLOR_GRAYSCALE;

HMENU Menus::menuBar;
HMENU Menus::fileMenu, Menus::emulationMenu, Menus::viewMenu;

HMENU Menus::CreateMainWindowMenus()
{
	Menus::menuBar = CreateMenu();

	// File menu
	fileMenu = CreateMenu();

	AppendMenu(fileMenu, MF_STRING, Menus::IDM_MAIN_FILE_LOAD_ROM, L"Load Cartridge");

	AppendMenu(fileMenu, MF_SEPARATOR, 0, NULL);

	AppendMenu(fileMenu, MF_STRING, IDM_MAIN_FILE_SAVE_STATE, L"Save State");
	AppendMenu(fileMenu, MF_STRING, IDM_MAIN_FILE_LOAD_STATE, L"Load State");

	AppendMenu(fileMenu, MF_SEPARATOR, 0, NULL);

	AppendMenu(fileMenu, MF_STRING, IDM_MAIN_FILE_QUIT, L"Quit");

	AppendMenu(menuBar, MF_POPUP, (UINT_PTR)fileMenu, L"File");


	// Emulation menu
	emulationMenu = CreateMenu();
	HMENU emulationSpeedMenu = CreateMenu();

	AppendMenu(emulationMenu, MF_STRING, IDM_MAIN_EMULATION_PAUSE, L"Pause");
	CheckMenuItem(emulationMenu, MF_STRING, MF_UNCHECKED);
	AppendMenu(emulationMenu, MF_STRING, IDM_MAIN_EMULATION_RESET, L"Reset");

	AppendMenu(emulationMenu, MF_SEPARATOR, 0, NULL);

	AppendMenu(emulationSpeedMenu, MF_STRING, IDM_MAIN_EMULATION_SPEED_1, L"x1");
	AppendMenu(emulationSpeedMenu, MF_STRING, IDM_MAIN_EMULATION_SPEED_2, L"x2");
	AppendMenu(emulationSpeedMenu, MF_STRING, IDM_MAIN_EMULATION_SPEED_5, L"x5");
	AppendMenu(emulationSpeedMenu, MF_STRING, IDM_MAIN_EMULATION_SPEED_10, L"x10");
	CheckMenuRadioItem(emulationSpeedMenu, IDM_MAIN_EMULATION_SPEED_1, IDM_MAIN_EMULATION_SPEED_10, IDM_MAIN_EMULATION_SPEED_1, MF_BYCOMMAND);
	AppendMenu(emulationMenu, MF_POPUP | MF_STRING, (UINT_PTR)emulationSpeedMenu, L"Speed");

	AppendMenu(menuBar, MF_POPUP, (UINT_PTR)emulationMenu, L"Emulation");


	// View menu
	viewMenu = CreateMenu();
	HMENU viewWindowSizeMenu = CreateMenu();
	HMENU viewColorMenu = CreateMenu();

	AppendMenu(viewWindowSizeMenu, MF_STRING, IDM_MAIN_VIEW_WINDOW_SIZE_100, L"100%");
	AppendMenu(viewWindowSizeMenu, MF_STRING, IDM_MAIN_VIEW_WINDOW_SIZE_200, L"200%");
	AppendMenu(viewWindowSizeMenu, MF_STRING, IDM_MAIN_VIEW_WINDOW_SIZE_300, L"300%");
	AppendMenu(viewWindowSizeMenu, MF_STRING, IDM_MAIN_VIEW_WINDOW_SIZE_400, L"400%");
	AppendMenu(viewWindowSizeMenu, MF_STRING, IDM_MAIN_VIEW_WINDOW_SIZE_500, L"500%");
	CheckMenuRadioItem(viewWindowSizeMenu, IDM_MAIN_VIEW_WINDOW_SIZE_100, IDM_MAIN_VIEW_WINDOW_SIZE_500, IDM_MAIN_VIEW_WINDOW_SIZE_200, MF_BYCOMMAND);
	AppendMenu(viewMenu, MF_POPUP | MF_STRING, (UINT_PTR)viewWindowSizeMenu, L"Window Size");

	AppendMenu(viewColorMenu, MF_STRING, IDM_MAIN_VIEW_COLOR_CLASSIC, L"Classic");
	AppendMenu(viewColorMenu, MF_STRING, IDM_MAIN_VIEW_COLOR_GRAYSCALE, L"Grayscale");
	CheckMenuRadioItem(viewColorMenu, IDM_MAIN_VIEW_COLOR_CLASSIC, IDM_MAIN_VIEW_COLOR_GRAYSCALE, IDM_MAIN_VIEW_COLOR_CLASSIC, MF_BYCOMMAND);
	AppendMenu(viewMenu, MF_POPUP | MF_STRING, (UINT_PTR)viewColorMenu, L"Colour Scheme");

	AppendMenu(viewMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(viewMenu, MF_STRING, IDM_MAIN_VIEW_TILES, L"Tile Viewer");
	CheckMenuItem(viewMenu, IDM_MAIN_VIEW_TILES, MF_UNCHECKED);

	AppendMenu(viewMenu, MF_STRING, IDM_MAIN_VIEW_DEBUG, L"Debugger");
	CheckMenuItem(viewMenu, IDM_MAIN_VIEW_DEBUG, MF_UNCHECKED);

	AppendMenu(menuBar, MF_POPUP, (UINT_PTR)viewMenu, L"View");


	return menuBar;
}

