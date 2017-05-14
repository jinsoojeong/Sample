#pragma once

#include "stdafx.h"
#include <strsafe.h> // ������ ��Ʈ�� �Լ����� ���� ���ؼ�

static void StringSplit(const std::string& original_string, char delim, std::vector<std::string>& elems)
{
	std::stringstream string(original_string);
	std::string split_string;

	while (std::getline(string, split_string, delim))
		elems.push_back(split_string);
}

static void StringSplit(const std::wstring& original_string, wchar_t delim, std::vector<std::wstring>& elems)
{
	std::wstringstream string(original_string);
	std::wstring split_string;

	while (std::getline(string, split_string, delim))
		elems.push_back(split_string);
}