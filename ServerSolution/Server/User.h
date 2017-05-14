#pragma once

class User : public MemoryPool<User>
{
public:
	User(DWORD id, std::wstring nick);
	virtual ~User() {}

	const DWORD GetID() const { return id_; }
	const std::wstring& GetNick() const { return nick_; }

private:
	DWORD id_;
	std::wstring nick_;
};