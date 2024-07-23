#include "WindowsApp.h"


WindowsApp::WindowsApp() : m_hwnd(NULL){}


LRESULT WindowsApp::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL WindowsApp::Create(HINSTANCE hInstance)
{
    const wchar_t CLASS_NAME[] = L"Windows Window Extension";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);


    m_hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Windows Window Extension Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL, NULL,
        hInstance,
        NULL
    );
    if (m_hwnd == NULL)
    {
        return 0;
    }

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
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
