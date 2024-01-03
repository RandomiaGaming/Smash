#pragma once

int hwstrlen(const wchar_t* str);
wchar_t* hwstrcpy(const wchar_t* source);
wchar_t* hwsubstr(const wchar_t* source, int start);
wchar_t* hwsubstr(const wchar_t* source, int start, int length);
wchar_t* hwstradd(const wchar_t* strA, wchar_t* strB);
wchar_t* hwtrim(const wchar_t* source);
wchar_t* hwtrimarg(const wchar_t* cmdline);
wchar_t* hwtrimquote(const wchar_t* source);