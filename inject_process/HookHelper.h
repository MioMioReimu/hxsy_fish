#pragma once
#ifdef HOOK_DLL_EXPORTS
#define HOOK_DLL_API __declspec(dllexport)
#else
#define HOOK_DLL_API 
#endif
HOOK_DLL_API class HookHelper
{
public:
	HOOK_DLL_API HookHelper();
	HOOK_DLL_API ~HookHelper();
	// @idHook : WH_CALLWNDPROC WH_CALLWNDPROCRET WH_CBT 
	// WH_DEBUG  WH_FOREGROUNDIDLE WH_GETMESSAGE WH_JOURNALPLAYBACK  WH_JOURNALRECORD WH_KEYBOARD
	// WH_KEYBOARD_LL  WH_MOUSE WH_MOUSE_LL WH_MSGFILTER WH_SHELL WH_SYSMSGFILTER
	HOOK_DLL_API BOOL hook(std::string hook_dll_entry_name, std::wstring hook_dll_name);
	HOOK_DLL_API DWORD get_process_id();
	HOOK_DLL_API BOOL set_process(std::wstring process_name);
	HOOK_DLL_API HWND get_wnd();
	HOOK_DLL_API DWORD get_main_thread_id();
	BOOL un_hook();
private:
	void set_process_id(DWORD process_id);
	void set_wnd(HWND hWnd);
	void set_main_thread_id(DWORD thread_id);
	friend BOOL CALLBACK enum_proc_for_window(HWND hWnd, LPARAM lParam);
	void get_main_window_hWnd(std::wstring name);
	DWORD m_process_id;
	DWORD m_main_thread_id;
	DWORD m_procs[1024];
	HWND m_hWnd;
	HHOOK m_hHook;
};

