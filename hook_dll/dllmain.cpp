// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "hook_dll.h"
#include "fish_window.h"
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	call_ins origin_call_ins;
	origin_call_ins.op = 0xE8;
	origin_call_ins.offset = MEM_CPY_POS - WND_HOOK_POS - 5;
	void * fish_mem_cpy_call_pos = (void*)WND_HOOK_POS;

	call_ins origin_txt_call_ins;
	origin_txt_call_ins.op = 0xE8;
	origin_txt_call_ins.offset = MEM_CPY_POS - TXT_HOOK_POS -5;
	void * txt_mem_cpy_call_pos = (void*)TXT_HOOK_POS;
	FILE*f;
	int ret = fopen_s(&f, "D:\\hook_dll.log", "a+");
	wchar_t s[255] = { 0 };
	wsprintf(s, L"hook_dll log file open error%d", ret);
	if (f == NULL)
		MessageBox(NULL, s, L"error", MB_OK);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		fprintf(f, "process_attach,process_id:%d\n", GetCurrentProcessId());
		break;
	case DLL_THREAD_ATTACH:
		fprintf(f, "thread_attach,thread_id:%d\n", GetCurrentThreadId());
		break;
	case DLL_THREAD_DETACH:

		fprintf(f, "thread_detach,thread_id:%d\n", GetCurrentThreadId());


		break;
	case DLL_PROCESS_DETACH:
		g_fish_mgr.stop_fish();
		if (g_created_thread)
		{
			DWORD oldProtect;
			VirtualProtectEx(GetCurrentProcess(), (LPVOID)fish_mem_cpy_call_pos, 256, PAGE_EXECUTE_READWRITE, &oldProtect);
			WriteProcessMemory(GetCurrentProcess(), fish_mem_cpy_call_pos, &origin_call_ins, sizeof(call_ins), NULL);
			VirtualProtectEx(GetCurrentProcess(), (LPVOID)fish_mem_cpy_call_pos, 256, oldProtect, NULL);

			VirtualProtectEx(GetCurrentProcess(), (LPVOID)txt_mem_cpy_call_pos, 256, PAGE_EXECUTE_READWRITE, &oldProtect);
			WriteProcessMemory(GetCurrentProcess(), txt_mem_cpy_call_pos, &origin_txt_call_ins, sizeof(call_ins), NULL);
			VirtualProtectEx(GetCurrentProcess(), (LPVOID)txt_mem_cpy_call_pos, 256, oldProtect, NULL);
		}
		fprintf(f, "process_detach,process_id:%d\n", GetCurrentProcessId());
		break;
	}
	fclose(f);
	return TRUE;
}

