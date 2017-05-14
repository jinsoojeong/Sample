#pragma once

namespace PT_Format
{
	struct REG_USER_REQ
	{
		std::wstring ticket;
	};

	struct REG_USER_ACK
	{
		REG_USER_ACK() : error_code(0), id(0), connect_user_count(0)
		{}

		BYTE error_code;

		DWORD id;
		std::wstring nick;
		DWORD connect_user_count;
		std::vector<std::wstring> connect_users;
	};

	struct CHECK_VERSION_REQ
	{
		std::wstring version;
	};

	struct CHECK_VERSION_ACK
	{
		CHECK_VERSION_ACK() : error_code(0)
		{}

		BYTE error_code;
	};

	struct UNREG_USER_REQ
	{
		DWORD id;
	};

	struct UNREG_USER_ACK
	{
		UNREG_USER_ACK() : error_code(0)
		{}

		BYTE error_code;
	};

	struct NEW_CONNECT_USER
	{
		std::wstring nick;
	};

	struct CHAT
	{
		std::wstring text;
	};

	struct DISCONNECT_USER
	{
		std::wstring nick;
	};
};
