#include <iostream>
#include <Windows.h>
#include <Sddl.h>
#include <aclapi.h>
#include "ministrlib.h"
#include "miniconlib.h"
using namespace std;

int main();
bool Smash(const wchar_t* target);
bool SubSmash(const wchar_t* target, PSID pEveryone);
bool SubSubSmash(const wchar_t* target, PSID pEveryone);
bool SetGodMode(bool enabled);
bool AdjustPrivlage(const wchar_t* privlage, bool enabled);

int main() {
	int output = 0;

	LPWSTR sysCommandLine = GetCommandLine();
	wcout << sysCommandLine << endl;

	int commandLength = wstrlen(sysCommandLine);
	wchar_t* command = wstrcpy(sysCommandLine);

	wchar_t* temp = wtrimarg(command);
	delete[] command;
	command = temp;
	commandLength = wstrlen(command);

	temp = wtrim(command);
	delete[] command;
	command = temp;
	commandLength = wstrlen(command);

	temp = wtrimquote(command);
	delete[] command;
	command = temp;
	commandLength = wstrlen(command);

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
		R(); wcerr << L"Error: Failed to enter GodMode." << endl; W();
		return false;
	}

	//Find the SID of the Everyone user.
	PSID pEveryone;
	if (!ConvertStringSidToSid(L"S-1-1-0", &pEveryone)) {
		Win32Error();
		R(); wcerr << L"Error: Failed to locate everyone security identifier." << endl; W();
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

	return SubSubSmash(target, pEveryone);
}
//Smashes a single file or folder non-recursively. Returns true if successful else false.
bool SubSubSmash(const wchar_t* target, PSID pEveryone) {
	/*//Smash the target file or folder
	DWORD result = SetNamedSecurityInfo(const_cast<LPWSTR>(target), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION, pEveryone, nullptr, nullptr, nullptr);
	if (result != ERROR_SUCCESS) {
		Win32Error(result);
		R(); wcerr << L"Error: Failed to set security info for \"" << target << "\"." << endl; W();
		return false;
	}
	wcout << "Smashed \"" << target << "\"." << endl;

	return true;*/

	//Delete the target file or folder
	DWORD attributes = GetFileAttributes(target);

	if (attributes == INVALID_FILE_ATTRIBUTES) {
		Win32Error();
		R(); wcerr << L"Error: Failed to get file attributes of \"" << target << "\"." << endl; W();
		return false;
	}

	if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
		if (RemoveDirectory(target) != 0) {
			wcout << "Destroyed folder \"" << target << "\"." << endl;
			return true;
		}
	}
	else {
		if (DeleteFile(target) != 0) {
			wcout << "Destroyed file \"" << target << "\"." << endl;
			return true;
		}
	}

	Win32Error();
	R(); wcerr << L"Error: Failed to delete file or folder \"" << target << "\"." << endl; W();
	return false;
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
		R(); wcerr << "Error: Unable to open process token for current process." << endl; W();
		return false;
	}

	LUID luidPrivlage;
	if (!LookupPrivilegeValue(NULL, privlage, &luidPrivlage)) {
		Win32Error();
		R(); wcerr << "Error: Unable to lookup privlage \"" << privlage << "\"." << endl; W();
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
		R();  wcerr << "Error: Unable to adjust privlage \"" << privlage << "\"." << endl; W();
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);
	return true;
}