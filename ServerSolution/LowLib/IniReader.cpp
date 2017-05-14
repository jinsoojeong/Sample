#include "stdafx.h"
#include "IniReader.h"

bool IniReader::Initialize(std::wstring& file_name)
{
	if (file_name_.empty() == false)
	{
		Log::ReportLog(L"already setting ini file name:[%s]", file_name_.c_str());
		return false;
	}
		

	if (file_name.empty())
	{
		Log::ReportLog(L"ini file name failed", file_name.c_str());
		return false;
	}

	file_name_= file_name.c_str();

	return true;
}

bool IniReader::LoadConfigData(std::wstring key_name, std::wstring value_name, DWORD *value)
{
	if (key_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety key_name:[%s]", key_name.c_str());
		return false;
	}

	if (value_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety value_name:[%S]", value_name.c_str());
		return false;
	}

	if (value == nullptr)
	{
		Log::ReportLog(L"Load Config Data Failed - invalid value pointer");
		return false;
	}

	*value = ::GetPrivateProfileInt(key_name.c_str(), value_name.c_str(), 0, file_name_.c_str());

	return true;
}

bool IniReader::LoadConfigData(std::wstring key_name, std::wstring value_name, float *value)
{
	if (key_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety key_name:[%s]", key_name.c_str());
		return false;
	}

	if (value_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety value_name:[%S]", value_name.c_str());
		return false;
	}

	if (value == nullptr)
	{
		Log::ReportLog(L"Load Config Data Failed - invalid value pointer");
		return false;
	}

	std::wstring value_str;
	DWORD buffer_length = 0;
	LoadConfigData(key_name, value_name, value_str, &buffer_length);

	*value = (float)_tstof(value_str.c_str());

	return true;
}

bool IniReader::LoadConfigData(std::wstring key_name, std::wstring value_name, std::wstring& value, DWORD *buffer_length)
{
	if (key_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety key_name:[%s]", key_name.c_str());
		return false;
	}

	if (value_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety value_name:[%S]", value_name.c_str());
		return false;
	}

	if (buffer_length == nullptr)
	{
		Log::ReportLog(L"Load Config Data Failed - invalid buffer_length pointer");
		return false;
	}

	wchar_t value_str[MAX_CONFIG_BUFFER] = { 0, };
	*buffer_length = ::GetPrivateProfileString(key_name.c_str(), value_name.c_str(), _T(""), value_str, *buffer_length, file_name_.c_str());
	value.resize(*buffer_length);
	value = value_str;

	return true;
}

bool IniReader::SetConfigData(std::wstring key_name, std::wstring value_name, DWORD value)
{
	if (key_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety key_name:[%s]", key_name.c_str());
		return false;
	}

	if (value_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety value_name:[%S]", value_name.c_str());
		return false;
	}

	std::wstring value_str;
	_stprintf(const_cast<wchar_t*>(value_str.c_str()), _T("%d"), value);

	::WritePrivateProfileString(key_name.c_str(), value_name.c_str(), value_str.c_str(), file_name_.c_str());

	return true;
}

bool IniReader::SetConfigData(std::wstring key_name, std::wstring value_name, std::wstring value)
{
	if (key_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety key_name:[%s]", key_name.c_str());
		return false;
	}

	if (value_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety value_name:[%S]", value_name.c_str());
		return false;
	}

	::WritePrivateProfileString(key_name.c_str(), value_name.c_str(), value.c_str(), file_name_.c_str());

	return true;
}

bool IniReader::SetConfigData(std::wstring key_name, std::wstring value_name, float value)
{
	if (key_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety key_name:[%s]", key_name.c_str());
		return false;
	}

	if (value_name.empty())
	{
		Log::ReportLog(L"Load Config Data Failed - Empety value_name:[%S]", value_name.c_str());
		return false;
	}

	std::wstring value_str;
	_stprintf(const_cast<wchar_t*>(value_str.c_str()), _T("%f"), value);

	::WritePrivateProfileString(key_name.c_str(), key_name.c_str(), value_str.c_str(), file_name_.c_str());

	return true;
}