#include <string>
#include <iostream>

#pragma once
class WinWinFunctions {
public:
	void Stack();
	void StackScroll();

	void Cascade();
	void Squish();
	
	void SaveWindowLayout();
	void ExecuteWindowLayout(std::string layout);
	
	void SaveDesktopLayout();
	void ExecuteDesktopLayout(std::string layout);

};