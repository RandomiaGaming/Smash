#pragma once

int wstrlen(const wchar_t* str);
wchar_t* wstrcpy(const wchar_t* source);
wchar_t* wsubstr(const wchar_t* source, int start);
wchar_t* wsubstr(const wchar_t* source, int start, int length);
wchar_t* wstradd(const wchar_t* strA, wchar_t* strB);
wchar_t* wtrim(const wchar_t* source);
wchar_t* wtrimarg(const wchar_t* cmdline);
wchar_t* wtrimquote(const wchar_t* source);