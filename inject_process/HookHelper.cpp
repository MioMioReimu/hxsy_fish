#pragma once 
#include "stdafx.h"
#include "HookHelper.h"
HookHelper::HookHelper()
{
}
DWORD HookHelper::get_process_id()
{ 
	return m_process_id;
}
HWND HookHelper::get_wnd()
{ 
	return m_hWnd; 
}
DWORD HookHelper::get_main_thread_id()
{ 
	return m_main_thread_id; 
}
void HookHelper::set_process_id(DWORD process_id)
{
	m_process_id = process_id;
}
void HookHelper::set_wnd(HWND hWnd)
{ 
	m_hWnd = hWnd;
}
void HookHelper::set_main_thread_id(DWORD thread_id)
{ 
	m_main_thread_id = thread_id; 
}
BOOL CALLBACK enum_proc_for_window(HWND hWnd, LPARAM lParam)
{
	DWORD dw_proc_id;
	DWORD dw_thread_id;
	dw_thread_id=GetWindowThreadProcessId(hWnd, &dw_proc_id);
	HookHelper* p_wnd_info = (HookHelper*)lParam;
	if (dw_proc_id == p_wnd_info->get_process_id() && GetParent(hWnd) == NULL){
		p_wnd_info->set_wnd(hWnd);
		p_wnd_info->set_main_thread_id(dw_thread_id);
		return FALSE;
	}
	return TRUE;
}
BOOL HookHelper::set_process(std::wstring process_name)
{
	get_main_window_hWnd(process_name);
	if (get_wnd() == NULL) return FALSE;
	return TRUE;
}
void HookHelper::get_main_window_hWnd(std::wstring name)
{
	DWORD procsnum = 0;
	TCHAR szEXEName[256] = { 0 };
	EnumProcesses(m_procs, 1024, &procsnum);
	for (int i = 0; i < procsnum; i++){
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_procs[i]);
		if (NULL != hProc){
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProc, &hMod,
				sizeof(hMod), &cbNeeded))
			{
				//Get the name of the exe file
				GetModuleBaseName(hProc, hMod, szEXEName,
					sizeof(szEXEName) / sizeof(TCHAR));
				wprintf(L"%s\n", szEXEName);
				if (wcscmp(szEXEName, name.data()) == 0)
				{
					this->set_process_id(m_procs[i]);
					this->set_wnd(NULL);
					CloseHandle(hProc);
					EnumWindows(enum_proc_for_window, (LPARAM)this);
					break;
				}
				CloseHandle(hProc);
			}
		}
	}
}

BOOL HookHelper::hook(std::string hook_dll_entry_name, std::wstring hook_dll_name)
{

	HINSTANCE h_dll_instance = LoadLibrary(hook_dll_name.data());
	if (NULL == h_dll_instance)
	{
		MessageBox(NULL, L"Load dll failed", L"Error",MB_OK);
		return FALSE;
	}
	FARPROC dll_entry_address = GetProcAddress(h_dll_instance,hook_dll_entry_name.data());
	if (NULL == dll_entry_address)
	{
		MessageBox(NULL, L"get dll entry failed", L"Error", MB_OK);
		return FALSE;
	}
	m_hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)dll_entry_address, h_dll_instance, this->get_main_thread_id());
	if (NULL == m_hHook)
	{
		MessageBox(NULL, L"set Hook failed!\n", L"Error", MB_OK);
		return FALSE;
	}
	FreeLibrary(h_dll_instance);
}
BOOL HookHelper::un_hook()
{
	if (m_hHook)
	{
		UnhookWindowsHookEx(m_hHook);
		m_hHook = NULL;
		return TRUE;
	}
	return FALSE;
}
		
HookHelper::~HookHelper()
{
}
