#include <string>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "resource.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <commctrl.h>
#include <sstream>
#include <Psapi.h>
#include <shellapi.h>
#include <algorithm>

#pragma once
static class WinWinFunctions {
public:

	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	static std::vector<HWND> GetActiveWindows();

	static void Stack(std::vector<HWND> WindowVect);
	void StackScroll();
	static void StackFourOrLess(std::vector<HWND> WindowVector);
	static void StackFiveToEight(std::vector<HWND> WindowVector);
	static void StackWindowsCallback(std::vector<HWND> WindowVect);

	static void Cascade(std::vector<HWND> WindowVect);
	static void Squish(std::vector<HWND> WindowVect);
	
	static void SaveWindowLayout(std::vector<HWND> WindowVect);
	static std::wstring GetUserInput(HINSTANCE hInstance);
	static void ExecuteWindowLayout(std::wstring json, std::vector<HWND> WindowVect);
	
	static void SaveDesktopLayout();
	static void ExecuteDesktopLayout(std::wstring json);

	static struct SavedWindow {
		SavedWindow(std::string process,
			UINT flags,
			UINT showCmd,
			POINT ptMinPosition,
			POINT ptMaxPosition,
			RECT rcNormalPosition) {
			m_process = process;
			m_flags = flags;
			m_showCmd = showCmd;
			m_ptMinPosition = ptMinPosition;
			m_ptMaxPosition = ptMaxPosition;
			m_rcNormalPosition = rcNormalPosition;
		};
		std::string m_process;
		UINT m_flags;
		UINT m_showCmd;
		POINT m_ptMinPosition;
		POINT m_ptMaxPosition;
		RECT m_rcNormalPosition;
		WINDOWPLACEMENT* getWinPlacement() {
			WINDOWPLACEMENT placement;
			placement.showCmd = m_showCmd;
			placement.flags = m_flags;
			placement.length = sizeof(WINDOWPLACEMENT);
			placement.ptMaxPosition = m_ptMinPosition;
			placement.ptMaxPosition = m_ptMaxPosition;
			placement.rcNormalPosition = m_rcNormalPosition;
			return &placement;
		}
	};

	struct SavedIcon {
		SavedIcon(std::string iconName,
			POINT iconPos) {
			m_iconName = iconName;
			m_iconPos = iconPos;
		};
		std::string m_iconName;
		POINT m_iconPos;
	};

	static std::string ConvertToNarrowString(const std::wstring& wstr) {
		if (wstr.empty()) {
			return std::string();
		}

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if (size_needed == 0) {
			throw std::runtime_error("WideCharToMultiByte failed");
		}

		std::string str(size_needed, 0);
		int result = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL);
		if (result == 0) {
			throw std::runtime_error("WideCharToMultiByte failed");
		}

		// Remove the null terminator added by WideCharToMultiByte
		str.resize(size_needed - 1);

		return str;
	}


};