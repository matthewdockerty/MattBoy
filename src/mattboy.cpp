#include <Windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

#include "gameboy/gameboy.hpp"
#include "util.hpp"


#define WINDOW_TITLE "MattBoy"
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

#define IDM_FILE_LOAD_ROM 1
#define IDM_FILE_SAVE_STATE 2
#define IDM_FILE_LOAD_STATE 3
#define IDM_FILE_OPTIONS 4
#define IDM_FILE_QUIT 5

#define IDM_EMULATION_PAUSE 6
#define IDM_EMULATION_RESET 7
#define IDM_EMULATION_SPEED_1 8
#define IDM_EMULATION_SPEED_2 9
#define IDM_EMULATION_SPEED_5 10
#define IDM_EMULATION_SPEED_10 11
#define IDM_EMULATION_TYPE_DMG 12
#define IDM_EMULATION_TYPE_COLOR 13
#define IDM_EMULATION_TYPE_ADVANCE 14

#define IDM_VIEW_WINDOW_SIZE_100 15
#define IDM_VIEW_WINDOW_SIZE_200 16
#define IDM_VIEW_WINDOW_SIZE_300 17
#define IDM_VIEW_WINDOW_SIZE_400 18
#define IDM_VIEW_WINDOW_SIZE_500 19
#define IDM_VIEW_DEBUG 20

const char *g_szClassName = "myWindowClass";
HMENU menu_bar;
HMENU file_menu;
HMENU emulation_menu, emulation_type_menu, emulation_speed_menu;
HMENU view_menu, view_window_size_menu;

std::atomic<bool> quit(false);

int screen[SCREEN_WIDTH * SCREEN_HEIGHT];
int scale_factor = 3;

mattboy::gameboy::Gameboy gameboy;
std::mutex mutex;

void AddMenus(HWND hwnd)
{
    menu_bar = CreateMenu();

    // File menu
    file_menu = CreateMenu();
    AppendMenu(file_menu, MF_STRING, IDM_FILE_LOAD_ROM, "Load ROM");
    AppendMenu(file_menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(file_menu, MF_STRING, IDM_FILE_SAVE_STATE, "Save State");
    AppendMenu(file_menu, MF_STRING, IDM_FILE_LOAD_STATE, "Load State");
    AppendMenu(file_menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(file_menu, MF_STRING, IDM_FILE_OPTIONS, "Options");
    AppendMenu(file_menu, MF_STRING, IDM_FILE_QUIT, "Quit");
    AppendMenu(menu_bar, MF_POPUP, (UINT_PTR)file_menu, "File");

    // Emulation menu
    emulation_menu = CreateMenu();
    emulation_type_menu = CreateMenu();
    emulation_speed_menu = CreateMenu();

    AppendMenu(emulation_menu, MF_STRING, IDM_EMULATION_PAUSE, "Pause");
    CheckMenuItem(emulation_menu, IDM_EMULATION_PAUSE, MF_UNCHECKED);
    AppendMenu(emulation_menu, MF_STRING, IDM_EMULATION_RESET, "Reset");

    AppendMenu(emulation_speed_menu, MF_STRING, IDM_EMULATION_SPEED_1, "x1");
    AppendMenu(emulation_speed_menu, MF_STRING, IDM_EMULATION_SPEED_2, "x2");
    AppendMenu(emulation_speed_menu, MF_STRING, IDM_EMULATION_SPEED_5, "x5");
    AppendMenu(emulation_speed_menu, MF_STRING, IDM_EMULATION_SPEED_10, "x10");
    CheckMenuRadioItem(emulation_speed_menu, IDM_EMULATION_SPEED_1, IDM_EMULATION_SPEED_10, IDM_EMULATION_SPEED_1, MF_BYCOMMAND);
    AppendMenu(emulation_menu, MF_POPUP | MF_STRING, (UINT_PTR)emulation_speed_menu, "Speed");

    AppendMenu(emulation_menu, MF_SEPARATOR, 0, NULL);

    AppendMenu(emulation_type_menu, MF_STRING, IDM_EMULATION_TYPE_DMG, "GameBoy (DMG)");
    AppendMenu(emulation_type_menu, MF_STRING, IDM_EMULATION_TYPE_COLOR, "GameBoy Color");
    AppendMenu(emulation_type_menu, MF_STRING, IDM_EMULATION_TYPE_ADVANCE, "GameBoy Advance");
    CheckMenuRadioItem(emulation_type_menu, IDM_EMULATION_TYPE_DMG, IDM_EMULATION_TYPE_ADVANCE, IDM_EMULATION_TYPE_DMG, MF_BYCOMMAND);
    AppendMenu(emulation_menu, MF_POPUP | MF_STRING, (UINT_PTR)emulation_type_menu, "Console Type");
    AppendMenu(menu_bar, MF_POPUP, (UINT_PTR)emulation_menu, "Emulation");

    // View menu
    view_menu = CreateMenu();
    view_window_size_menu = CreateMenu();
    AppendMenu(view_window_size_menu, MF_STRING, IDM_VIEW_WINDOW_SIZE_100, "100%");
    AppendMenu(view_window_size_menu, MF_STRING, IDM_VIEW_WINDOW_SIZE_200, "200%");
    AppendMenu(view_window_size_menu, MF_STRING, IDM_VIEW_WINDOW_SIZE_300, "300%");
    AppendMenu(view_window_size_menu, MF_STRING, IDM_VIEW_WINDOW_SIZE_400, "400%");
    AppendMenu(view_window_size_menu, MF_STRING, IDM_VIEW_WINDOW_SIZE_500, "500%");
    CheckMenuRadioItem(view_window_size_menu, IDM_VIEW_WINDOW_SIZE_100, IDM_VIEW_WINDOW_SIZE_500, IDM_VIEW_WINDOW_SIZE_200, MF_BYCOMMAND);
    AppendMenu(view_menu, MF_POPUP | MF_STRING, (UINT_PTR)view_window_size_menu, "Window Size");
    AppendMenu(view_menu, MF_STRING, IDM_VIEW_DEBUG, "Debugger");
    CheckMenuItem(view_menu, IDM_VIEW_DEBUG, MF_UNCHECKED);
    AppendMenu(menu_bar, MF_POPUP, (UINT_PTR)view_menu, "View");

    SetMenu(hwnd, menu_bar);
}

void SetWindowScale(HWND hwnd, int scale)
{
    scale_factor = scale;
    unsigned int width = scale_factor * SCREEN_WIDTH;
    unsigned int height = scale_factor * SCREEN_HEIGHT;

    RECT client, window;
    POINT diff;
    GetClientRect(hwnd, &client);
    GetWindowRect(hwnd, &window);
    diff.x = (window.right - window.left) - client.right;
    diff.y = (window.bottom - window.top) - client.bottom;
    MoveWindow(hwnd, window.left, window.top, width + diff.x, height + diff.y, TRUE);
}

void LoadROM(HWND hwnd)
{
  OPENFILENAME ofn;       // common dialog box structure
  TCHAR szFile[100] = { 0 };       // if using TCHAR macros
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = "GameBoy ROM\0*.gb;*.rom\0\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  if (GetOpenFileName(&ofn) == TRUE)
  {
    std::vector<char> rom;
    const std::string file = std::string(ofn.lpstrFile);
    if(mattboy::ReadFile(file, rom))
    {
      mutex.lock();

      gameboy.LoadCartridge(rom, file);
      if (gameboy.GetCartridge()->IsValid())
      {
        SetWindowTextA(hwnd, std::string(WINDOW_TITLE).append(": ").append(gameboy.GetCartridge()->GetTitle()).c_str());
        gameboy.SetRunning(true);
      }
      else
      {
        MessageBox ( NULL , "Unable to load cartridge. It is either invalid or unsupported at this time.", "Invalid Cartridge!" , MB_OK);
        gameboy.SetRunning(false);
      }

      mutex.unlock();
    }
    else
    {
      	MessageBox ( NULL , "Unable to load ROM!", "Error" , MB_OK);
    }
  }
}

void MenuItemClick(HWND hwnd, WPARAM wParam)
{
  UINT state;

  switch (wParam)
  {
  case IDM_FILE_LOAD_ROM:
    LoadROM(hwnd);
    break;
  case IDM_FILE_SAVE_STATE:
    std::cout << "TODO: Save State" << std::endl;
    break;
  case IDM_FILE_LOAD_STATE:
    std::cout << "TODO: Load State" << std::endl;
    break;
  case IDM_FILE_OPTIONS:
    std::cout << "TODO: Options" << std::endl;
    break;
  case IDM_FILE_QUIT:
    SendMessage(hwnd, WM_CLOSE, 0, 0);
    break;
  case IDM_EMULATION_PAUSE:
    state = GetMenuState(emulation_menu, IDM_EMULATION_PAUSE, MF_BYCOMMAND);
    if (state == MF_CHECKED)
    {
      mutex.lock();
      gameboy.SetRunning(true);
      mutex.unlock();
      CheckMenuItem(emulation_menu, IDM_EMULATION_PAUSE, MF_UNCHECKED);
    }
    else
    {
      mutex.lock();
      gameboy.SetRunning(false);
      mutex.unlock();
      CheckMenuItem(emulation_menu, IDM_EMULATION_PAUSE, MF_CHECKED);
    }
    break;
  case IDM_EMULATION_RESET:
    mutex.lock();
    gameboy.Reset();
    mutex.unlock();
    break;
  case IDM_EMULATION_SPEED_1:
    CheckMenuRadioItem(emulation_speed_menu, IDM_EMULATION_SPEED_1, IDM_EMULATION_SPEED_10, IDM_EMULATION_SPEED_1, MF_BYCOMMAND);
    std::cout << "TODO: Emulation speed x1" << std::endl;
    break;
  case IDM_EMULATION_SPEED_2:
    CheckMenuRadioItem(emulation_speed_menu, IDM_EMULATION_SPEED_1, IDM_EMULATION_SPEED_10, IDM_EMULATION_SPEED_2, MF_BYCOMMAND);
    std::cout << "TODO: Emulation speed x2" << std::endl;
    break;
  case IDM_EMULATION_SPEED_5:
    CheckMenuRadioItem(emulation_speed_menu, IDM_EMULATION_SPEED_1, IDM_EMULATION_SPEED_10, IDM_EMULATION_SPEED_5, MF_BYCOMMAND);
    std::cout << "TODO: Emulation speed x5" << std::endl;
    break;
  case IDM_EMULATION_SPEED_10:
    CheckMenuRadioItem(emulation_speed_menu, IDM_EMULATION_SPEED_1, IDM_EMULATION_SPEED_10, IDM_EMULATION_SPEED_10, MF_BYCOMMAND);
    std::cout << "TODO: Emulation speed x10" << std::endl;
    break;
  case IDM_EMULATION_TYPE_DMG:
    std::cout << "TODO: Set DMG" << std::endl;
    CheckMenuRadioItem(emulation_type_menu, IDM_EMULATION_TYPE_DMG, IDM_EMULATION_TYPE_ADVANCE, IDM_EMULATION_TYPE_DMG, MF_BYCOMMAND);
    break;
  case IDM_EMULATION_TYPE_COLOR:
    std::cout << "TODO: Set GB Color" << std::endl;
    CheckMenuRadioItem(emulation_type_menu, IDM_EMULATION_TYPE_DMG, IDM_EMULATION_TYPE_ADVANCE, IDM_EMULATION_TYPE_COLOR, MF_BYCOMMAND);
    break;
  case IDM_EMULATION_TYPE_ADVANCE:
    std::cout << "TODO: Set GB Advance" << std::endl;
    CheckMenuRadioItem(emulation_type_menu, IDM_EMULATION_TYPE_DMG, IDM_EMULATION_TYPE_ADVANCE, IDM_EMULATION_TYPE_ADVANCE, MF_BYCOMMAND);
    break;
  case IDM_VIEW_WINDOW_SIZE_100:
    SetWindowScale(hwnd, 1);
    CheckMenuRadioItem(view_window_size_menu, IDM_VIEW_WINDOW_SIZE_100, IDM_VIEW_WINDOW_SIZE_500, IDM_VIEW_WINDOW_SIZE_100, MF_BYCOMMAND);
    break;
  case IDM_VIEW_WINDOW_SIZE_200:
    SetWindowScale(hwnd, 2);
    CheckMenuRadioItem(view_window_size_menu, IDM_VIEW_WINDOW_SIZE_100, IDM_VIEW_WINDOW_SIZE_500, IDM_VIEW_WINDOW_SIZE_200, MF_BYCOMMAND);
    break;
  case IDM_VIEW_WINDOW_SIZE_300:
    SetWindowScale(hwnd, 3);
    CheckMenuRadioItem(view_window_size_menu, IDM_VIEW_WINDOW_SIZE_100, IDM_VIEW_WINDOW_SIZE_500, IDM_VIEW_WINDOW_SIZE_300, MF_BYCOMMAND);
    break;
  case IDM_VIEW_WINDOW_SIZE_400:
    SetWindowScale(hwnd, 4);
    CheckMenuRadioItem(view_window_size_menu, IDM_VIEW_WINDOW_SIZE_100, IDM_VIEW_WINDOW_SIZE_500, IDM_VIEW_WINDOW_SIZE_400, MF_BYCOMMAND);
    break;
  case IDM_VIEW_WINDOW_SIZE_500:
    SetWindowScale(hwnd, 5);
    CheckMenuRadioItem(view_window_size_menu, IDM_VIEW_WINDOW_SIZE_100, IDM_VIEW_WINDOW_SIZE_500, IDM_VIEW_WINDOW_SIZE_500, MF_BYCOMMAND);
    break;
  case IDM_VIEW_DEBUG:
    state = GetMenuState(view_menu, IDM_VIEW_DEBUG, MF_BYCOMMAND);
    if (state == MF_CHECKED)
    {
      std::cout << "TODO: Hide debugger" << std::endl;
      CheckMenuItem(view_menu, IDM_VIEW_DEBUG, MF_UNCHECKED);
    }
    else
    {
      std::cout << "TODO: Show debugger" << std::endl;
      CheckMenuItem(view_menu, IDM_VIEW_DEBUG, MF_CHECKED);
    }
    break;
  }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CLOSE:
    DestroyWindow(hwnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_CREATE:
    AddMenus(hwnd);
    break;
  case WM_COMMAND:
    MenuItemClick(hwnd, wParam);
    break;
  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}


void Loop(HWND hwnd)
{
  HDC hdc = GetDC(hwnd);

  BITMAPINFO bmi = { 
    sizeof(BITMAPINFOHEADER),
    SCREEN_WIDTH,
    -SCREEN_HEIGHT,
    1,
    32
  };

  int val = 0;

  while (!quit)
  {
    mutex.lock();
    gameboy.Cycle();
    mutex.unlock();

    val++;
    val %= 3;

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
      screen[i] = rand() % 0xFFFFFF;
    }

    StretchDIBits(hdc, 0, 0, SCREEN_WIDTH * scale_factor, SCREEN_HEIGHT * scale_factor, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, screen, &bmi, 0, SRCCOPY);
  }

  ReleaseDC(hwnd, hdc);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  WNDCLASSEX wc;
  HWND hwnd;
  MSG msg;

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = g_szClassName;
  wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

  if (!RegisterClassEx(&wc))
  {
    MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  // Step 2: Creating the Window
  hwnd = CreateWindowEx(
    WS_EX_CLIENTEDGE,
    g_szClassName,
    WINDOW_TITLE,
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT,
    NULL, NULL, hInstance, NULL);

  if (hwnd == NULL)
  {
    MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  SetWindowScale(hwnd, scale_factor);
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
  {
    screen[i] = rand() % 0xFFFFFF;
  }

  std::thread gb_thread(Loop, hwnd);
  while (!quit)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
        quit = true;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  gb_thread.join();

  return msg.wParam;
}