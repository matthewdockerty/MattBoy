#pragma once

#include <Windows.h>
#include <string>
#include <memory>
#include <functional>
#include <map>

class GUIWindow
{
public:
	typedef std::function<void(UINT_PTR)> const MenuCallback;

	GUIWindow(LPCWSTR title, int width, int height, HWND parent, HMENU menu, MenuCallback menuCallback, HINSTANCE hInstance, bool show, std::shared_ptr<bool>& quit);
	~GUIWindow();

	void MessageProcess();

	void SetScale(int scale);
	HWND GetHwnd();
	void SetVisible(bool visible);
	void DisplayPixels(const int* pixels);
	void SetTitle(LPCWSTR title);

private:
	static std::map<HWND, MenuCallback> menuCallbacks;

	const LPCWSTR WINDOW_CLASS_NAME = L"MattBoyWindowClass";

	WNDCLASSEX wc_;
	HWND hwnd_;
	HWND parent_;
	int width_, height_, scale_;
	
	BITMAPINFO bmi_;
	HDC hdc_;

	std::shared_ptr<bool> quit_;

	void CreateWin(LPCWSTR title, HINSTANCE hInstance, HMENU menu);
	void RegisterWinClass(HINSTANCE hInstance);

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

