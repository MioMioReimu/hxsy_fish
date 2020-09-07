// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HOOK_DLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HOOK_DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once
#ifdef HOOK_DLL_EXPORTS
#define HOOK_DLL_API __declspec(dllexport)
#else
#define HOOK_DLL_API __declspec(dllimport)
#endif
#define WND_HOOK_POS 0x007EEE6F
#define TXT_HOOK_POS 0x0041768F
#define MEM_CPY_POS 0x00415AF0
#include "fish_window.h"
#pragma pack(push,1)
struct call_ins
{
	unsigned char op;
	int offset;
};
#pragma pack(pop)
struct hwnd_info
{
	DWORD process_id;
	DWORD main_thread_id;
	HWND main_hWnd;
};
extern hwnd_info h_wnd_info;
extern BOOL g_created_thread;
extern fish_mgr g_fish_mgr;
extern BOOL CALLBACK enum_proc_for_window(HWND hWnd, LPARAM lParam);
extern void get_main_window_hWnd(std::wstring name);
extern "C" DWORD WINAPI thread_main(LPVOID param);
extern "C" HOOK_DLL_API LRESULT  CALLBACK hook_entry(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
