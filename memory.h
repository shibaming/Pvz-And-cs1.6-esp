#pragma once
#include <Windows.h>
#include<vector>
class Memory
{
public:
	Memory():m_handle(0){}
	Memory(HANDLE handle)
	{
		m_handle = handle;
	}
	template<typename T>
	T read(uintptr_t lpBaseAddress)
	{
		T t;
		ReadProcessMemory(m_handle, (LPCVOID)lpBaseAddress,&t, sizeof t, nullptr);
		return t;
	}
	template<typename T>
	void read_list(uintptr_t lpBaseAddress,T* t,int count)
	{
		ReadProcessMemory(m_handle, (LPCVOID)lpBaseAddress, t, count * (sizeof T), nullptr);
	}
	template<typename T>
	void read_chain(uintptr_t lpBaseAddress, std::vector<uintptr_t> vec, T& out)
	{
		for (int i = 0; i < vec.size(); i++)
		{
			if (i + 1 == vec.size());
			{
				out = read<T>(lpBaseAddress + vec[i]);
			}
			lpBaseAddress = read<int>(lpBaseAddress + vec[i]);
		}
	}
	HANDLE m_handle;
};

