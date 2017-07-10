#pragma once
using namespace PT_Format;

inline void READ_PT_REG_USER_REQ(Msg *msg, REG_USER_REQ &parameter)
{
	*msg >> parameter.ticket;
}

inline void READ_PT_REG_USER_ACK(Msg *msg, REG_USER_ACK &parameter)
{
	*msg >> &parameter.error_code;
	*msg >> &parameter.id;
	*msg >> parameter.nick;
	*msg >> &parameter.connect_user_count;
	
	parameter.connect_users.resize(parameter.connect_user_count);

	for (DWORD i = 0; i < parameter.connect_user_count; i++)
		*msg >> parameter.connect_users[i];
}

inline void READ_PT_CHECK_VERSION_REQ(Msg *msg, CHECK_VERSION_REQ &parameter)
{
	*msg >> parameter.version;
}

inline void READ_PT_CHECK_VERSION_ACK(Msg *msg, CHECK_VERSION_ACK &parameter)
{
	*msg >> &parameter.error_code;
}

inline void READ_PT_UNREG_USER_REQ(Msg *msg, UNREG_USER_REQ &parameter)
{
	*msg >> &parameter.id;
}

inline void READ_PT_UNREG_USER_ACK(Msg *msg, UNREG_USER_ACK &parameter)
{
	*msg >> &parameter.error_code;
}

inline void READ_PT_NEW_CONNECT_USER(Msg *msg, NEW_CONNECT_USER &parameter)
{
	*msg >> parameter.nick;
}

inline void READ_PT_DISCONNECT_USER(Msg *msg, DISCONNECT_USER &parameter)
{
	*msg >> parameter.nick;
}

inline void READ_PT_CHAT(Msg *msg, CHAT &parameter)
{
	*msg >> parameter.text;
}