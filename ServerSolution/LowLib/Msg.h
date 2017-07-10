#pragma once
#include "MemoryPool.h"

class IMsg
{
public:
	IMsg() {};
	virtual ~IMsg() {};

	virtual bool ReadDWORD(DWORD *data) = 0;
	virtual bool ReadULONGLONG(ULONGLONG *data) = 0;
	virtual bool ReadByte(BYTE *data) = 0;
	virtual bool ReadBytes(BYTE *data) = 0;
	virtual bool ReadUSHORT(USHORT *data) = 0;
	virtual bool ReadSHORT(SHORT *data) = 0;

	virtual bool ReadInt32(int *data) = 0;
	virtual bool ReadInt64(__int64 *data) = 0;
	virtual bool ReadFloat(float *data) = 0;
	virtual bool ReadBOOL(bool *data) = 0;
	virtual bool ReadWCHAR(wchar_t *data) = 0;
	virtual bool ReadWSTR(std::wstring& data) = 0;

	virtual bool WriteDWORD(DWORD data) = 0;
	virtual bool WriteULONGLONG(ULONGLONG data) = 0;
	virtual bool WriteByte(BYTE data) = 0;
	virtual bool WriteBytes(BYTE *data) = 0;
	virtual bool WriteUSHORT(USHORT data) = 0;
	virtual bool WriteSHORT(SHORT data) = 0;

	virtual bool WriteInt32(int data) = 0;
	virtual bool WriteInt64(__int64 data) = 0;
	virtual bool WriteFloat(float data) = 0;
	virtual bool WriteBOOL(bool data) = 0;
	virtual bool WriteWCHAR(wchar_t data) = 0;
	virtual bool WriteWSTR(const std::wstring& data) = 0;

	// -------------------- WRITE --------------------
	IMsg& operator << (const DWORD data)
	{
		WriteDWORD(data);
		return *this;
	}

	IMsg& operator << (const ULONGLONG data)
	{
		WriteULONGLONG(data);
		return *this;
	}

	IMsg& operator << (const BYTE data)
	{
		WriteByte(data);
		return *this;
	}

	IMsg& operator << (BYTE *data)
	{
		WriteBytes(data);
		return *this;
	}

	IMsg& operator << (const SHORT data)
	{
		WriteSHORT(data);
		return *this;
	}

	IMsg& operator << (const int data)
	{
		WriteInt32(data);
		return *this;
	}

	IMsg& operator << (const __int64 data)
	{
		WriteInt64(data);
		return *this;
	}

	IMsg& operator << (const float data)
	{
		WriteFloat(data);
		return *this;
	}

	IMsg& operator << (const bool data)
	{
		WriteBOOL(data);
		return *this;
	}

	IMsg& operator << (const wchar_t data)
	{
		WriteWCHAR(data);
		return *this;
	}
	
	IMsg& operator << (const std::wstring& data)
	{
		WriteWSTR(data);
		return *this;
	}

	// -------------------- READ --------------------

	IMsg& operator >> (DWORD *data)
	{
		ReadDWORD(data);
		return *this;
	}

	IMsg& operator >> (ULONGLONG *data)
	{
		ReadULONGLONG(data);
		return *this;
	}

	IMsg& operator >> (BYTE *data)
	{
		if (sizeof(*data) == 1)
			ReadByte(data);
		else
			ReadBytes(data);

		return *this;
	}

	IMsg& operator >> (SHORT *data)
	{
		ReadSHORT(data);
		return *this;
	}

	IMsg& operator >> (int *data)
	{
		ReadInt32(data);
		return *this;
	}

	IMsg& operator >> (__int64 *data)
	{
		ReadInt64(data);
		return *this;
	}

	IMsg& operator >> (float *data)
	{
		ReadFloat(data);
		return *this;
	}

	IMsg& operator >> (bool *data)
	{
		ReadBOOL(data);
		return *this;
	}

	IMsg& operator >> (wchar_t *data)
	{
		ReadWCHAR(data);
		return *this;
	}

	IMsg& operator >> (std::wstring& data)
	{
		ReadWSTR(data);
		return *this;
	}
};

class Msg : public MemoryPool<Msg>, public IMsg
{
public:
	Msg(DWORD protocol_id);
	virtual ~Msg() {};

private:
	bool ReadDWORD(DWORD *data);
	bool ReadULONGLONG(ULONGLONG *data);
	bool ReadByte(BYTE *data);
	bool ReadBytes(BYTE *data);
	bool ReadUSHORT(USHORT *data);
	bool ReadSHORT(SHORT *data);

	bool ReadInt32(int *data);
	bool ReadInt64(__int64 *data);
	bool ReadFloat(float *data);
	bool ReadBOOL(bool *data);
	bool ReadWCHAR(wchar_t *data);
	bool ReadWSTR(std::wstring& data);

	bool WriteDWORD(DWORD data);
	bool WriteULONGLONG(ULONGLONG data);
	bool WriteByte(BYTE data);
	bool WriteBytes(BYTE *data);
	bool WriteUSHORT(USHORT data);
	bool WriteSHORT(SHORT data);

	bool WriteInt32(int data);
	bool WriteInt64(__int64 data);
	bool WriteFloat(float data);
	bool WriteBOOL(bool data);
	bool WriteWCHAR(wchar_t data);
	bool WriteWSTR(const std::wstring& data);

public:
	const DWORD	GetLength();
	const DWORD	GetID();
	const BYTE*	GetBuffer();

	void SetBuffer(BYTE* buffer) { buffer_ = buffer; }
	void SetSessionID(DWORD session_id) { session_id_ = session_id; }
	DWORD GetSessionID() { return session_id_; }

private:
	DWORD session_id_;
	DWORD offset_;
	DWORD protocol_id_;
	BYTE *buffer_;
};