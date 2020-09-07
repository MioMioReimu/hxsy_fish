#pragma once
#include "stdafx.h"
#include "HookHelper.h"
#include <errno.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include "my_cegui_window.h"
void main()
{
	HookHelper s;
	printf("current process id:%d\n", GetCurrentProcessId());
	printf("current thread id:%d\n", GetCurrentThreadId());
	s.set_process(L"game.bin");
	s.hook("hook_entry", L"hook_dll");
	//Sleep(30000);
	
	std::map<int,  int,std::less<int>> m_t;
	for (int i = 0; i < 200; i++)
	{
		m_t.insert(std::pair<int, int>(i, i<<4));
	}
	std::list<int> m_l;
	for (int i = 0; i < 200; i++)
	{
		m_l.push_back(i);
	}
	//std::cout 
	//	<< "\tcegui_size:" << sizeof(cegui_window) << std::endl;
	//cegui_window *m;
	//void *pointer_window_name_ptr = (void*)0x0127c4c0;
	int m = // (cegui_window*)((unsigned char*)pointer_window_name_ptr - 108)
		(size_t)(&(((cegui_window*)0x0)->parent));
	std::cout<<std::hex << (int)m << std::endl;
	system("pause");
}