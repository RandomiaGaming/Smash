#pragma once
#include <Windows.h>
#include "ministrlib.h"

static class MiniCon {
public:
	MiniCon();
	~MiniCon();
	void Write(const wchar_t* message);
	void WriteWarning(const wchar_t* message);
	void WriteError(const wchar_t* message);
	void Return();
	void Win32Error();
	void Win32Error(DWORD errorCode);
private:
	HANDLE conHandle = 0;
};