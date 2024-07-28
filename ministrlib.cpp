#include "ministrlib.h";
int wstrlen(const wchar_t* str) {
	if (str == nullptr) {
		return 0;
	}
	int length = 0;
	while (str[length] != L'\0') {
		length++;
	}
	return length;
}
wchar_t* wstrcpy(const wchar_t* source) {
	if (source == nullptr) {
		return nullptr;
	}
	int length = wstrlen(source);
	wchar_t* copy = new wchar_t[length + 1];
	for (int i = 0; i < length; i++)
	{
		copy[i] = source[i];
	}
	copy[length] = '\0';
	return copy;
}
wchar_t* wsubstr(const wchar_t* source, int start) {
	if (source == nullptr) {
		return nullptr;
	}
	if (start < 0) {
		start = 0;
	}
	int sourceLength = wstrlen(source);
	int substringLength = sourceLength - start;
	if (substringLength < 0) {
		substringLength = 0;
	}
	wchar_t* substr = new wchar_t[substringLength + 1];
	for (int i = 0; i < substringLength; i++)
	{
		substr[i] = source[start + i];
	}
	substr[substringLength] = L'\0';
	return substr;
}
wchar_t* wsubstr(const wchar_t* source, int start, int length) {
	if (source == nullptr) {
		return nullptr;
	}
	if (start < 0) {
		length += start;
		start = 0;
	}
	int sourceLength = wstrlen(source);
	if (length < 0) {
		length = 0;
	}
	else if (start + length > sourceLength) {
		length = sourceLength - start;
	}
	wchar_t* substr = new wchar_t[length + 1];
	for (int i = 0; i < length; i++)
	{
		substr[i] = source[start + i];
	}
	substr[length] = L'\0';
	return substr;
}
wchar_t* wstradd(const wchar_t* strA, wchar_t* strB) {
	if (strA == nullptr) {
		return wstrcpy(strB);
	}
	else if (strB == nullptr) {
		return wstrcpy(strA);
	}
	int lengthA = wstrlen(strA);
	int lengthB = wstrlen(strB);
	int outputLength = lengthA + lengthB + 1;
	wchar_t* output = new wchar_t[outputLength];
	for (int i = 0; i < lengthA; i++)
	{
		output[i] = strA[i];
	}
	for (int i = 0; i < lengthB; i++)
	{
		output[lengthA + i] = strA[i];
	}
	output[outputLength] = L'\0';
	return output;
}
wchar_t* wtrim(const wchar_t* source) {
	if (source == nullptr) {
		return nullptr;
	}
	int sourceLength = wstrlen(source);
	int start = 0;
	int end = sourceLength - 1;
	while (start < sourceLength) {
		wchar_t c = source[start];
		if (c != L' ' && c != L'\n' && c != L'\r' && c != L'\t') {
			break;
		}
		start++;
	}
	while (end > start) {
		wchar_t c = source[end];
		if (c != L' ' && c != L'\n' && c != L'\r' && c != L'\t') {
			break;
		}
		end--;
	}
	return wsubstr(source, start, end - start + 1);
}
wchar_t* wtrimarg(const wchar_t* cmdline) {
	if (cmdline == nullptr) {
		return nullptr;
	}
	int length = wstrlen(cmdline);
	bool foundChar = false;
	bool inQuotes = false;
	for (int i = 0; i < length; i++) {
		if (cmdline[i] == L' ') {
			if (!inQuotes && foundChar) {
				if (i + 1 < length) {
					return wsubstr(cmdline, i + 1);
				}
				else {
					return wstrcpy(L"");
				}
			}
		}
		else if (cmdline[i] == L'"') {
			if (inQuotes) {
				if (i + 1 < length) {
					return wsubstr(cmdline, i + 1);
				}
				else {
					return wstrcpy(L"");
				}
			}
			else {
				if (foundChar) {
					return wsubstr(cmdline, i);
				}
				else {
					inQuotes = true;
				}
			}
		}
		else {
			foundChar = true;
		}
	}
	return wstrcpy(L"");
}
wchar_t* wtrimquote(const wchar_t* source)
{
	if (source == nullptr) {
		return nullptr;
	}
	int length = wstrlen(source);
	if (length == 1 && source[0] == L'\"') {
		return wstrcpy(L"");
	}
	else if (length >= 2 && source[0] == L'"')
	{
		if (source[length - 1] == L'"')
		{
			return wsubstr(source, 1, length - 2);
		}
		else
		{
			return wsubstr(source, 1);
		}
	}
	else {
		return wstrcpy(source);
	}
}