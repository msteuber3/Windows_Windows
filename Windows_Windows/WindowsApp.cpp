#include "WindowsApp.h"


WindowsApp::WindowsApp() : m_hwnd(NULL){}


LRESULT WindowsApp::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

BOOL WindowsApp::Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
	return 0;
}

HWND WindowsApp::Window()
{
	return HWND();
}

PCWSTR WindowsApp::ClassName() const
{
	return PCWSTR();
}

LRESULT WindowsApp::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}
