#include <iostream>
#include <Windows.h>
#include <Sddl.h>
#include <aclapi.h>
#include "strh.h"
using namespace std;
#pragma warning(disable : 4996)

int main();

bool Smash(const wchar_t* target);

bool SubSmash(const wchar_t* target, PSID pEveryone);
bool SetGodMode(bool enabled);
bool AdjustPrivlage(const wchar_t* privlage, bool enabled);
void Win32Error();
void Win32Error(DWORD errorCode);

const wchar_t* redCode = L"\033[1;31m";
const wchar_t* resetCode = L"\033[0m";

int main() {
	int output = 0;

	LPWSTR sysCommandLine = GetCommandLine();
	
	int commandLength = hwstrlen(sysCommandLine);
	wchar_t* command = hwstrcpy(sysCommandLine);

	wchar_t* temp = hwtrimarg(command);
	delete[] command;
	command = temp;
	commandLength = hwstrlen(command);

	temp = hwtrim(command);
	delete[] command;
	command = temp;
	commandLength = hwstrlen(command);

	temp = hwtrimquote(command);
	delete[] command;
	command = temp;
	commandLength = hwstrlen(command);

	if (commandLength == 0)
	{
		wcout << endl;
		wcout << L"Smash - Version 1.0.0" << endl;
		wcout << endl;
		wcout << L"Usage: Smash C:\\Path\\To\\Some\\File.txt" << endl;
		wcout << L"Usage: Smash C:\\Path\\To\\Some\\Folder" << endl;
		wcout << endl;
		wcout << L"Smashing a file or folder will preform the following actions:" << endl;
		wcout << L"Set the owner to the Everyone group." << endl;
		wcout << L"Disable permission inheritance." << endl;
		wcout << L"Remove all permission entries." << endl;
		wcout << L"Add a permission entry granting full control to the Everyone group." << endl;
		wcout << L"For folders sub-folders, and sub-files will also be smashed." << endl;
		output = 0;
	}
	else
	{
		if (Smash(command)) {
			output = 0;
		}
		else {
			output = 1;
		}
	}

	delete[] command;
	return output;
}

//Prepares to smash target then uses SubSmash. Returns true if successful else false.
bool Smash(const wchar_t* target) {
	//Enter GodMode
	if (!SetGodMode(true)) {
		wcerr << redCode << L"Error: Failed to enter GodMode." << resetCode << endl;
		return false;
	}

	//Find the SID of the Everyone user.
	PSID pEveryone;
	if (!ConvertStringSidToSid(L"S-1-1-0", &pEveryone)) {
		Win32Error();
		wcerr << redCode << L"Error: Failed to locate everyone security identifier." << resetCode << endl;
		SetGodMode(false);
		return false;
	}

	bool subResult = SubSmash(target, pEveryone);

	//Cleanup
	LocalFree(pEveryone);
	SetGodMode(false);
	return subResult;
}
//Smashes a file or folder recursively. Returns true if successful else false.
bool SubSmash(const wchar_t* target, PSID pEveryone) {
	//Smash the target file or folder
	DWORD result = SetNamedSecurityInfo(const_cast<LPWSTR>(target), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION, pEveryone, nullptr, nullptr, nullptr);
	if (result != ERROR_SUCCESS) {
		Win32Error(result);
		wcerr << redCode << L"Error: Failed to set security info for \"" << target << "\"." << resetCode << endl;
		return false;
	}
	wcout << "Smashed: \"" << target << "\"." << endl;

	//Calculate subSelector
	int targetLength = lstrlen(target);
	wchar_t* subSelector = new wchar_t[targetLength + 3];
	for (int i = 0; i < targetLength; i++)
	{
		subSelector[i] = target[i];
	}
	subSelector[targetLength] = L'\\';
	subSelector[targetLength + 1] = L'*';
	subSelector[targetLength + 2] = L'\0';

	//Find subfiles and smash them
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(subSelector, &findFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (lstrcmp(findFileData.cFileName, L".") != 0 && lstrcmp(findFileData.cFileName, L"..") != 0)) {
				//Calculate subTarget
				int fileNameLength = lstrlen(findFileData.cFileName);
				int subTargetLength = targetLength + fileNameLength + 1;
				wchar_t* subTarget = new wchar_t[subTargetLength + 1];
				for (int i = 0; i < targetLength; i++)
				{
					subTarget[i] = target[i];
				}
				subTarget[targetLength] = L'\\';
				for (int i = 0; i < fileNameLength; i++)
				{
					subTarget[i + targetLength + 1] = findFileData.cFileName[i];
				}
				subTarget[subTargetLength] = L'\0';

				//Smash sub folder or sub file.
				SubSmash(subTarget, pEveryone);

				//Cleanup
				delete[] subTarget;
			}
		} while (FindNextFile(hFind, &findFileData) != 0);
		//Cleanup
		FindClose(hFind);
	}
	//Cleanup
	delete[] subSelector;
}
//Grants or revokes GodMode from the current process. Returns true if successful else false.
bool SetGodMode(bool enabled) {
	if (!AdjustPrivlage(SE_RESTORE_NAME, enabled)) {
		return false;
	}
	if (!AdjustPrivlage(SE_BACKUP_NAME, enabled)) {
		return false;
	}
	return true;
}
//Grants or revokes a privlage from the current process. Returns true if successful else false.
bool AdjustPrivlage(const wchar_t* privlage, bool enabled) {
	HANDLE hProcess = GetCurrentProcess();

	HANDLE hToken;
	if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		Win32Error();
		wcerr << redCode << "Error: Unable to open process token for current process." << resetCode << endl;
		return false;
	}

	LUID luidPrivlage;
	if (!LookupPrivilegeValue(NULL, privlage, &luidPrivlage)) {
		Win32Error();
		wcerr << redCode << "Error: Unable to lookup privlage \"" << privlage << "\"." << endl;
		CloseHandle(hToken);
		return false;
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luidPrivlage;
	if (enabled) {
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else {
		tp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;
	}

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		Win32Error();
		wcerr << redCode << "Error: Unable to adjust privlage \"" << privlage << "\"." << endl;
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);
	return true;
}
//Prints the error code and error message from the last win32 error to the standard error stream.
void Win32Error() {
	DWORD errorCode = GetLastError();

	LPVOID errorMessage;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
	FormatMessage(flags, nullptr, errorCode, 0, reinterpret_cast<LPWSTR>(&errorMessage), 0, nullptr);
	std::wcerr << redCode << L"Win32Error " << errorCode << L": " << reinterpret_cast<LPWSTR>(errorMessage) << resetCode << endl;
	LocalFree(errorMessage);
}
//Prints the error code and error message from the specified error code to the standard error stream.
void Win32Error(DWORD errorCode) {
	LPVOID errorMessage;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
	FormatMessage(flags, nullptr, errorCode, 0, reinterpret_cast<LPWSTR>(&errorMessage), 0, nullptr);
	std::wcerr << redCode << L"Win32Error " << errorCode << L": " << reinterpret_cast<LPWSTR>(errorMessage) << resetCode;
	LocalFree(errorMessage);
}