#include <Windows.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <istream>
#include <iterator>
#include <locale>
#include <codecvt>
#include <string>
#include <thread>

#include "../MattBoy/Gameboy.h"

#include "GUIWindow.h"
#include "Menus.h"


std::shared_ptr<bool> quit;
std::unique_ptr<GUIWindow> mainWindow, tilesWindow;
std::unique_ptr<mattboy::Gameboy> gameboy;

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

bool ReadFile(const std::wstring& filename, std::vector<char>& result)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file)
		return false;

	file.unsetf(std::ios::skipws); // don't consume newlines

	std::streampos filesize;

	file.seekg(0, std::ios::end);
	filesize = file.tellg();
	file.seekg(0, std::ios::beg);

	result.reserve(filesize);
	result.insert(result.begin(), std::istream_iterator<char>(file), std::istream_iterator<char>());

	file.close();

	return true;
}

void LoadROM()
{
	OPENFILENAME ofn;
	TCHAR szFile[100] = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = mainWindow->GetHwnd();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"GameBoy ROM\0*.gb;*.rom\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		std::vector<char> rom;
		const std::wstring file(ofn.lpstrFile);
		if (ReadFile(file, rom))
		{
			gameboy->LoadCartridge(rom, file);
			if (gameboy->GetCartridge()->IsValid())
			{
				std::string title = "MattBoy Classic - " + gameboy->GetCartridge()->GetTitle();
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				std::wstring wTitle = converter.from_bytes(title);

				mainWindow->SetTitle(wTitle.c_str());
				gameboy->SetRunning(true);
			}
			else
			{
				MessageBox(NULL, L"Unable to load cartridge. It is either invalid or unsupported at this time.", L"Invalid Cartridge!", MB_OK);
				gameboy->SetRunning(false);
			}
		}
		else
		{
			MessageBox(NULL, L"Unable to load ROM!", L"Error", MB_OK);
		}
	}
}

void MainWindowMenuCallback(UINT_PTR itemId)
{
	UINT state;

	switch (itemId)
	{

	case Menus::IDM_MAIN_FILE_LOAD_ROM: LoadROM(); break;
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
		if (state == MF_CHECKED)
		{
			tilesWindow->SetVisible(false);
			CheckMenuItem(Menus::viewMenu, Menus::IDM_MAIN_VIEW_TILES, MF_UNCHECKED);
		}
		else
		{
			tilesWindow->DisplayPixels(gameboy->GetTileViewPixels(true));
			tilesWindow->SetVisible(true);
			CheckMenuItem(Menus::viewMenu, Menus::IDM_MAIN_VIEW_TILES, MF_CHECKED);
		}

		break;
	case Menus::IDM_MAIN_VIEW_DEBUG: break;
	case Menus::IDM_MAIN_VIEW_COLOR_CLASSIC: break;
	case Menus::IDM_MAIN_VIEW_COLOR_GRAYSCALE: break;

	default:
		break;
	}
}

void GameboyLoop()
{
	auto last_time = std::chrono::high_resolution_clock::now();
	int cycles = 0;

	while (!*quit)
	{
		auto current_time = std::chrono::high_resolution_clock::now();
		auto time = current_time - last_time;
		auto ns = time / std::chrono::nanoseconds(1);
		if (ns >= mattboy::CPU::NANOSECONDS_PER_CLOCK * cycles)
		{
			last_time = current_time;
			cycles = gameboy->Cycle();
		}
	}

	gameboy->SetRunning(false);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CreateConsoleWindow();

	quit = std::make_shared<bool>(false);

	mainWindow = std::make_unique<GUIWindow>(L"MattBoy Classic", mattboy::GPU::SCREEN_WIDTH, mattboy::GPU::SCREEN_HEIGHT, (HWND)NULL, Menus::CreateMainWindowMenus(), MainWindowMenuCallback, hInstance, true, quit);
	mainWindow->SetScale(2);

	tilesWindow = std::make_unique<GUIWindow>(L"Tile Viewer", mattboy::MMU::TILE_VIEW_WIDTH, mattboy::MMU::TILE_VIEW_HEIGHT, mainWindow->GetHwnd(), (HMENU)NULL, nullptr, hInstance, false, quit);
	tilesWindow->SetScale(2);
	gameboy = std::make_unique<mattboy::Gameboy>();

	std::thread gameboyThread(GameboyLoop);

	while (!*quit)
	{
		mainWindow->MessageProcess();
		tilesWindow->MessageProcess();

		if (gameboy->IsRunning() && gameboy->HasTileViewChanged())
			tilesWindow->DisplayPixels(gameboy->GetTileViewPixels(true));
	}

	gameboyThread.join();

	CleanUp();
}
