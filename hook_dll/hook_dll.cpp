// hook_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "hook_dll.h"
int g_created_thread = 0;
fish_mgr g_fish_mgr;
HOOK_DLL_API LRESULT CALLBACK hook_entry(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	FILE*f;
	int ret = fopen_s(&f,"D:\\operation.log", "a+");
	wchar_t s[255] = { 0 };
	wsprintf(s, L"operate file open error%d", ret);
	if (f == NULL)
		MessageBox(NULL, s, L"error", MB_OK);
	if(f!=NULL)fprintf(f, "operate mouse\n");
	if (f != NULL)fclose(f);
	if (g_created_thread == 0)
	{
		thread_main(NULL);
		g_created_thread = 1;
	}
	return CallNextHookEx(NULL,nCode, wParam, lParam);
}

FILE * mem_set_log_file;
uint32_t fish_pointer_count = 0;
void __stdcall window_filter_process(void *dst, const void *src, size_t size);
void __stdcall draw_text_filter_process(void *dst, const void *src, size_t size);
__declspec(naked) void __stdcall draw_window_mem_cpy(void *dst, const void* src, size_t size)
{
	__asm
	{
		push ebp;
		mov ebp, esp;
		//push esi;
	}
	window_filter_process(dst, src, size);
	__asm
	{
		//pop esi;
		mov ebx, MEM_CPY_POS;
		push[ebp + 0x10];
		push[ebp + 0x0C];
		push[ebp + 0x8];
		call ebx;
		add esp, 0x0C;
		pop ebp;
		retn;
	}
}
__declspec(naked) void __stdcall draw_test_mem_cpy(void *dst, const void* src, size_t size)
{
	__asm
	{
		push ebp;
		mov ebp, esp;
		//push esi;
	}
	draw_text_filter_process(dst, src, size);
	__asm
	{
		//pop esi;
		push eax;
		mov eax, MEM_CPY_POS;
		push[ebp + 0x10];
		push[ebp + 0x0C];
		push[ebp + 0x8];
		call eax;
		pop eax;
		add esp, 0x0C;
		pop ebp;
		retn;
	}
}

void __stdcall window_filter_process(void*dst, const void*src, size_t size)
{
	static int pointer_count = 0;
	if (mem_set_log_file != NULL)
	{
		if (0 == wcsncmp((const wchar_t *)src, L"Root/Fishing/BattleBar/Pointer", 30))
		{
			++pointer_count;
			if (pointer_count == 4)
			{
				g_fish_mgr.load_fish(dst);
			}
			if (7 == pointer_count)
			{
				pointer_count = 0;
			}
			//	}
			//	++fish_pointer_count;
		}
		//++fish_pointer_count;
		//fwprintf(mem_set_log_file, L"%d_%s\n", fish_pointer_count,(const wchar_t *)src);
	}
}
void fish_set_fishing_state_thread_proc()
{
	Sleep(1000);
	g_fish_mgr.set_state(fish_mgr::INITED);
}
void __stdcall draw_text_filter_process(void *dst, const void *src, size_t size)
{
	static int fish_txt_count = 0;
	if (0 == strncmp((const char*)src, "好像钩到了什么东西", 16))
	{
		if (fish_txt_count == 0)
		{
			g_fish_mgr.set_state(fish_mgr::INITED);
			//boost::thread t(fish_set_fishing_state_thread_proc);
			fwprintf(mem_set_log_file, L"start fish:thing\n");
			fish_txt_count = 1;
		}
	}
	if (0 == strncmp((const char*)src, "好像是条普通的小鱼", 16))
	{
		if (fish_txt_count == 0)
		{
			g_fish_mgr.set_state(fish_mgr::INITED);
			//boost::thread t(fish_set_fishing_state_thread_proc);
			fwprintf(mem_set_log_file, L"start fish:little fish\n");
			fish_txt_count = 1;
		}
	}
	else if (0 == strncmp((const char*)src, "好像是条还不错的大鱼", 18))
	{
		if (fish_txt_count == 0)
		{
			g_fish_mgr.set_state(fish_mgr::INITED);
			//boost::thread t(fish_set_fishing_state_thread_proc);
			fwprintf(mem_set_log_file, L"start fish:big fish\n");
			fish_txt_count = 1;
		}
	}
	else if (0 == strncmp((const char*)src, "好像是条非常有力的大鱼", 20))
	{
		if (fish_txt_count == 0)
		{
			g_fish_mgr.set_state(fish_mgr::INITED);
			//boost::thread t(fish_set_fishing_state_thread_proc);
			fwprintf(mem_set_log_file, L"start fish:very big fish\n");
			fish_txt_count = 1;
		}
	}
	else if (0 == strncmp((const char*)src, "你钓起了", 8))
	{
		if (fish_txt_count == 1){
			g_fish_mgr.set_state(fish_mgr::COMPLETED);
			fwprintf(mem_set_log_file, L"complete fish\n");
			fish_txt_count = 0;
		}
	}
	else if (0 == strncmp((const char*)src, "你尚未装备钓饵", 14))
	{
		g_fish_mgr.set_state(fish_mgr::LOADED);
		g_fish_mgr.stop_fish();
		fish_txt_count = 0;
	}
}
DWORD WINAPI thread_main(LPVOID param)
{
	int ret = fopen_s(&mem_set_log_file, "D:\\mem_set.log", "a+");
	wchar_t s[255] = { 0 };
	wsprintf(s, L"mem_set file open error%d", ret);
	if (mem_set_log_file == NULL)
		MessageBox(NULL, s, L"error", MB_OK);

	get_main_window_hWnd(L"game.bin");

	void * mem_cpy_pos = (void*)MEM_CPY_POS;
	void * window_mem_cpy_call_pos = (void*)WND_HOOK_POS;
	DWORD oldProtect;

	call_ins hooked_window_mem_call;
	hooked_window_mem_call.op = 0xE8;
	hooked_window_mem_call.offset = (int)draw_window_mem_cpy - (int)window_mem_cpy_call_pos - 5;
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)window_mem_cpy_call_pos, 256, PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(GetCurrentProcess(), window_mem_cpy_call_pos, &hooked_window_mem_call, sizeof(call_ins), NULL);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)window_mem_cpy_call_pos, 256, oldProtect, NULL);

	void *text_mem_cpy_call_pos = (void*)TXT_HOOK_POS;
	call_ins hooked_text_mem_call;
	hooked_text_mem_call.op = 0xE8;
	hooked_text_mem_call.offset = (int)draw_test_mem_cpy - (int)text_mem_cpy_call_pos - 5;
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)text_mem_cpy_call_pos, 256, PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(GetCurrentProcess(), text_mem_cpy_call_pos, &hooked_text_mem_call, sizeof(call_ins), NULL);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)text_mem_cpy_call_pos, 256, oldProtect, NULL);
	
	//Sleep(3000);
	g_fish_mgr.start_fish();
	return 1;
}