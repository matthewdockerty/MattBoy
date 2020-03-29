#include "GUIWindow.h"


std::map<HWND, GUIWindow::MenuCallback> GUIWindow::menuCallbacks;

GUIWindow::GUIWindow(LPCSTR title, int width, int height, HWND parent, HMENU menu, MenuCallback menuCallback, HINSTANCE hInstance, bool show, std::shared_ptr<bool>& quit) : width_(width), height_(height), scale_(1), parent_(parent), quit_(quit)
{
	if (parent == NULL)
		RegisterWinClass(hInstance);
	
	CreateWin(title, hInstance, menu);


	if (show)
		ShowWindow(hwnd_, SW_SHOW);

	menuCallbacks.insert(std::make_pair(hwnd_, menuCallback));
}

GUIWindow::~GUIWindow()
{
}

void GUIWindow::RegisterWinClass(HINSTANCE hInstance)
{
	wc_.cbSize = sizeof(WNDCLASSEX);
	wc_.style = 0;
	wc_.lpfnWndProc = WndProc;
	wc_.cbClsExtra = 0;
	wc_.cbWndExtra = 0;
	wc_.hInstance = hInstance;
	wc_.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc_.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc_.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc_.lpszMenuName = NULL;
	wc_.lpszClassName = WINDOW_CLASS_NAME;
	wc_.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc_))
	{
		MessageBox(NULL, "Failed to register window", "Error", MB_ICONERROR | MB_OK);
		exit(1);
	}
}

void GUIWindow::CreateWin(LPCSTR title, HINSTANCE hInstance, HMENU menu)
{
	hwnd_ = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WINDOW_CLASS_NAME,
		title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | (parent_ == NULL ? WS_MINIMIZEBOX : 0),
		CW_USEDEFAULT, CW_USEDEFAULT, width_, height_,
		parent_, menu, hInstance, NULL);

	if (hwnd_ == NULL)
	{
		MessageBox(NULL, "Failed to create window", "Error", MB_ICONERROR | MB_OK);
		exit(1);
	}

	SetScale(scale_);
}

void GUIWindow::MessageProcess()
{
	MSG msg;

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			*quit_ = true;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK GUIWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		// Hide child window instead of closing
		if (GetWindow(hwnd, GW_OWNER) != NULL)
			ShowWindow(hwnd, SW_HIDE);
		else
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_COMMAND:
		{
			auto result = menuCallbacks.find(hwnd);
			if (result != menuCallbacks.end())
			{
				result->second(wParam);
			}
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

HWND GUIWindow::GetHwnd()
{
	return hwnd_;
}

void GUIWindow::SetScale(int scale)
{
	scale_ = scale;
	unsigned int width = scale_ * width_;
	unsigned int height = scale_ * height_;

	RECT client, window;
	POINT diff;
	GetClientRect(hwnd_, &client);
	GetWindowRect(hwnd_, &window);
	diff.x = (window.right - window.left) - client.right;
	diff.y = (window.bottom - window.top) - client.bottom;
	MoveWindow(hwnd_, window.left, window.top, width + diff.x, height + diff.y, TRUE);
}