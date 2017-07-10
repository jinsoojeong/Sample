#pragma once

#define MAX_CONFIG_BUFFER 16

class IniReader
{
public:
	IniReader() {};
	virtual ~IniReader() {};

	bool Initialize(std::wstring& file_name);

	bool LoadConfigData(std::wstring key_name, std::wstring value_name, std::wstring& value, DWORD *buffer_length);
	bool LoadConfigData(std::wstring key_name, std::wstring value_name, DWORD *value);
	bool LoadConfigData(std::wstring key_name, std::wstring value_name, float *value);

	bool SetConfigData(std::wstring key_name, std::wstring value_name, std::wstring value);
	bool SetConfigData(std::wstring key_name, std::wstring value_name, DWORD value);
	bool SetConfigData(std::wstring key_name, std::wstring value_name, float value);

private:
	std::wstring file_name_;
};
