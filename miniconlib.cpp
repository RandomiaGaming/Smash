#include "miniconlib.h"

MiniCon::MiniCon() {
	conHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}
void MiniCon::Write(const wchar_t* message) {
	WriteConsole(conHandle, message, wstrlen(message), nullptr, nullptr);
}
void MiniCon::WriteWarning(const wchar_t* message) {

	WriteConsole(conHandle, message, wstrlen(message), nullptr, nullptr);
}
void MiniCon::WriteError(const wchar_t* message) {
	SetConsoleTextAttribute(conHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
	WriteConsole(conHandle, message, wstrlen(message), nullptr, nullptr);
}
void MiniCon::Win32Error() {
	DWORD errorCode = GetLastError();

	LPVOID errorMessage;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
	FormatMessage(flags, nullptr, errorCode, 0, reinterpret_cast<LPWSTR>(&errorMessage), 0, nullptr);
	SetConsoleTextAttribute(conHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
	WriteConsole(conHandle, L"Win32Error ", 11, nullptr, nullptr);
	WriteConsole(conHandle, L"Win32Error ", 11, nullptr, nullptr);
	WriteConsole(conHandle, L"Win32Error ", 11, nullptr, nullptr);
	WriteConsole(conHandle, L"Win32Error ", 11, nullptr, nullptr);
	LocalFree(errorMessage);
}
void MiniCon::Win32Error(DWORD errorCode) {
	LPVOID errorMessage;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
	FormatMessage(flags, nullptr, errorCode, 0, reinterpret_cast<LPWSTR>(&errorMessage), 0, nullptr);
	R(); wcerr << L"Win32Error " << errorCode << L": " << reinterpret_cast<LPWSTR>(errorMessage) << endl; W();
	LocalFree(errorMessage);
}
void MiniCon::R() {
	
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
}
void MiniCon::W() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}