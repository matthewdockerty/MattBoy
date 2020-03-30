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

	GUIWindow(LPCSTR title, int width, int height, HWND parent, HMENU menu, MenuCallback menuCallback, HINSTANCE hInstance, bool show, std::shared_ptr<bool>& quit);
	~GUIWindow();

	void MessageProcess();

	void SetScale(int scale);
	HWND GetHwnd();
	void SetVisible(bool visible);

private:
	static std::map<HWND, MenuCallback> menuCallbacks;

	const LPCSTR WINDOW_CLASS_NAME = "MattBoyWindowClass";

	WNDCLASSEX wc_;
	HWND hwnd_;
	HWND parent_;
	int width_, height_, scale_;

	std::shared_ptr<bool> quit_;

	
	void CreateWin(LPCSTR title, HINSTANCE hInstance, HMENU menu);
	void RegisterWinClass(HINSTANCE hInstance);

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

