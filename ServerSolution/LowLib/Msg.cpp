#include "stdafx.h"
#include "ThreadSync.h"
#include "Msg.h"

Msg::Msg(DWORD protocol_id) : session_id_(0), offset_(0), protocol_id_(protocol_id)
{
	buffer_ = new BYTE[MAX_BUFFER_LENGTH];
}

bool Msg::ReadDWORD(DWORD *data)
{
	memcpy(data, buffer_ + offset_, sizeof(DWORD));
	offset_ += sizeof(DWORD);

	return true;
}

bool Msg::ReadULONGLONG(ULONGLONG *data)
{
	memcpy(data, buffer_ + offset_, sizeof(ULONGLONG));
	offset_ += sizeof(ULONGLONG);

	return true;
}

bool Msg::ReadByte(BYTE *data)
{
	memcpy(data, buffer_ + offset_, sizeof(BYTE));
	offset_ += sizeof(BYTE);

	return true;
}

bool Msg::ReadBytes(BYTE *data)
{
	DWORD wLen = 0;
	ReadDWORD(&wLen);

	memcpy(data, buffer_ + offset_, wLen);
	offset_ += wLen;

	return true;
}

bool Msg::ReadSHORT(SHORT *data)
{
	memcpy(data, buffer_ + offset_, sizeof(SHORT));
	offset_ += sizeof(SHORT);

	return true;
}

bool Msg::ReadUSHORT(USHORT *data)
{
	memcpy(data, buffer_ + offset_, sizeof(USHORT));
	offset_ += sizeof(USHORT);

	return true;
}

bool Msg::ReadInt32(int *data)
{
	memcpy(data, buffer_ + offset_, sizeof(int));
	offset_ += sizeof(int);

	return true;
}

bool Msg::ReadInt64(__int64 *data)
{
	memcpy(data, buffer_ + offset_, sizeof(__int64));
	offset_ += sizeof(__int64);

	return true;
}

bool Msg::ReadFloat(float *data)
{
	memcpy(data, buffer_ + offset_, sizeof(float));
	offset_ += sizeof(float);

	return true;
}

bool Msg::ReadBOOL(bool *data)
{
	memcpy(data, buffer_ + offset_, sizeof(bool));
	offset_ += sizeof(bool);

	return true;
}

bool Msg::ReadWCHAR(wchar_t *data)
{
	memcpy(data, buffer_ + offset_, sizeof(wchar_t));
	offset_ += sizeof(wchar_t);

	return true;
}

bool Msg::ReadWSTR(std::wstring& data)
{
	DWORD wLen = 0;
	ReadDWORD(&wLen);

	data.resize(wLen);

	memcpy((void*)data.c_str(), buffer_ + offset_, wLen * sizeof(wchar_t));
	offset_ += wLen * sizeof(wchar_t);

	return true;
}

bool Msg::WriteDWORD(DWORD data)
{
	memcpy( buffer_ + offset_, &data, sizeof(DWORD));
	offset_ += sizeof(DWORD);

	return true;
}

bool Msg::WriteULONGLONG(ULONGLONG data)
{
	memcpy( buffer_ + offset_, &data, sizeof(ULONGLONG));
	offset_ += sizeof(ULONGLONG);

	return true;
}

bool Msg::WriteByte(BYTE data)
{
	memcpy( buffer_ + offset_, &data, sizeof(BYTE));
	offset_ += sizeof(BYTE);

	return true;
}

bool Msg::WriteBytes(BYTE *data)
{
	DWORD wLen = sizeof(data);
	WriteDWORD(wLen);

	memcpy( buffer_ + offset_, data, wLen);
	offset_ += wLen;

	return true;
}

bool Msg::WriteSHORT(SHORT data)
{
	memcpy(buffer_ + offset_, &data, sizeof(SHORT));
	offset_ += sizeof(SHORT);

	return true;
}

bool Msg::WriteUSHORT(USHORT data)
{
	memcpy(buffer_ + offset_, &data, sizeof(USHORT));
	offset_ += sizeof(USHORT);

	return true;
}

bool Msg::WriteInt32(int data)
{
	memcpy(buffer_ + offset_, &data, sizeof(int));
	offset_ += sizeof(int);

	return true;
}

bool Msg::WriteInt64(__int64 data)
{
	memcpy(buffer_ + offset_, &data, sizeof(__int64));
	offset_ += sizeof(__int64);

	return true;
}

bool Msg::WriteFloat(float data)
{
	memcpy( buffer_ + offset_, &data, sizeof(float));
	offset_ += sizeof(float);

	return true;
}

bool Msg::WriteBOOL(bool data)
{
	memcpy( buffer_ + offset_, &data, sizeof(bool));
	offset_ += sizeof(bool);

	return true;
}

bool Msg::WriteWCHAR(const wchar_t data)
{
	memcpy( buffer_ + offset_, &data, sizeof(wchar_t));
	offset_ += sizeof(wchar_t);

	return true;
}

bool Msg::WriteWSTR(const std::wstring& data)
{
	DWORD wLen = (data.c_str() == nullptr) ? (0) : DWORD(::lstrlenW(data.c_str()));
	WriteDWORD(wLen);

	memcpy( buffer_ + offset_, (void*)data.c_str(), wLen * sizeof(wchar_t));
	offset_ += wLen * sizeof(wchar_t);

	return true;
}

const BYTE* Msg::GetBuffer()
{
	return buffer_;
}

const DWORD Msg::GetLength()
{
	return offset_;
}

const DWORD Msg::GetID()
{
	return protocol_id_;
}
