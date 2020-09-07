#pragma once
#include "stdafx.h"
#include "fish_window.h"
#include "hook_dll.h"
extern FILE *mem_set_log_file;
fish_window::fish_window()
{
	m_fish_battle_window = nullptr;
	m_pointer_window = nullptr;
	memset(m_progress_bar_windows, 0, 7 * sizeof(cegui_window *));
	memset(m_each_progress_bar_width, 0, 7 * sizeof(float *));
	m_pointer_rect = nullptr;
	m_callback = nullptr;
}
bool fish_window::init_window()
{
	if (m_pointer_window != nullptr){
		m_fish_battle_window = m_pointer_window->parent;
		m_progress_bar_windows[0] = m_fish_battle_window->childs.list_ptr[1];
		m_progress_bar_windows[1] = m_fish_battle_window->childs.list_ptr[3];
		m_progress_bar_windows[2] = m_fish_battle_window->childs.list_ptr[4];
		m_progress_bar_windows[3] = m_fish_battle_window->childs.list_ptr[5];
		m_progress_bar_windows[4] = m_fish_battle_window->childs.list_ptr[6];
		m_progress_bar_windows[5] = m_fish_battle_window->childs.list_ptr[7];
		m_progress_bar_windows[6] = m_fish_battle_window->childs.list_ptr[2];

		for (int i = 0; i < 7; ++i)
			m_each_progress_bar_width[i] = m_progress_bar_windows[i]->d_pixelSize;
		m_pointer_rect = &m_pointer_window->d_outerUnclippedRect;

		m_min_pointer_value = 0.5*(m_progress_bar_windows[0]->d_outerUnclippedRect.d_right +
			m_progress_bar_windows[0]->d_outerUnclippedRect.d_left);
		m_increased_progress_bar_width[0] = *m_each_progress_bar_width[0];
		for (int i = 1; i < 7; ++i)
		{
			m_increased_progress_bar_width[i] = m_increased_progress_bar_width[i - 1]
				+ *m_each_progress_bar_width[i];
		}
	}
	return true;
}
bool fish_window::set_window(void *pointer_window_name_ptr)
{
	m_pointer_window = (cegui_window*)((unsigned char*)pointer_window_name_ptr-108)
		;//(size_t)(&(((cegui_window*)0x0)->name));
	
	
	return true;
}
fish_window_callback* fish_window::set_callback(fish_window_callback * callback)
{
	fish_window_callback * old_fish_callback;
	old_fish_callback = m_callback;
	m_callback = callback;
	return old_fish_callback;
}
fish_mgr::fish_mgr()
{
	m_state = UNDEFINED;
	m_fish_thread_started = false;
	m_thread_handle = nullptr;
	m_callback = new fish_window_process();
	fish_window_callback *c = m_fish_window.set_callback(m_callback);
	if (c != nullptr)
		delete c;
}
bool fish_window::update_window()
{
	if (m_fish_battle_window != nullptr){
		if (m_callback)
		{
			fwprintf(mem_set_log_file, L"leftborder: %x\t", (unsigned int)m_each_progress_bar_width[0] - (unsigned )m_progress_bar_windows[0]);
			m_callback->process_fish_callback((const float**)m_each_progress_bar_width,
				m_increased_progress_bar_width,
				0.5*(m_pointer_rect->d_left+m_pointer_rect->d_right)
				- m_min_pointer_value);
		}
	}
	return true;
}
//this function should be called by window_filter_process
bool fish_mgr::load_fish(void *pointer_window_name_ptr)
{
	m_fish_window.set_window(pointer_window_name_ptr);
	m_state = LOADED;
	return true;
}
bool fish_mgr::init_fish()
{
	this->m_fish_window.init_window();
	return true;
}

//开始钓鱼监控线程，
bool fish_mgr::start_fish()
{
	if (!m_fish_thread_started)
	{
		boost::mutex::scoped_lock s_lock(m_lock_thread_started);
		if (!m_fish_thread_started){
			assert(m_thread_handle == nullptr);
			m_fish_thread_started = true;
			m_thread_handle = new boost::thread(boost::bind(&fish_mgr::fish_thread_proc, this));
		}
	}
	return true;
}
//关闭钓鱼监控线程
bool fish_mgr::stop_fish()
{
	if (m_thread_handle)
	{
		//boost::mutex::scoped_lock s_lock(m_lock_thread_started);
		m_fish_thread_started = false;
		assert(m_thread_handle != nullptr);
		//m_thread_handle->join();
		m_thread_handle->detach();
		Sleep(200);
		delete m_thread_handle;
		m_thread_handle = nullptr;
	}
	return true;
}
//设置游戏中的状态 由draw_text_filter_process调用
bool fish_mgr::set_state(state fish_state)
{
	m_state = fish_state;
	return true;
}
void fish_mgr::fish_thread_proc()
{
	POINT cursor_pos;
	INPUT ip;
	while (m_fish_thread_started)
	{
		//m_lock_thread_started.lock();
		//if (!m_fish_thread_started)
		//{
			//m_lock_thread_started.unlock();
		//	break;
		//}
		//m_lock_thread_started.unlock();
		switch (m_state)
		{
		case UNDEFINED:
			Sleep(100);
			break;
		case LOADED:
			Sleep(100);
			break;
		case INITED:
			Sleep(1000);
			
			GetCursorPos(&m_cursor_pos);
			ip.type = INPUT_MOUSE;
			ip.mi.dwExtraInfo = GetMessageExtraInfo();
			ip.mi.mouseData = 0;
			ip.mi.time = 0;
			ip.mi.dx = m_cursor_pos.x;
			ip.mi.dy = m_cursor_pos.y;

			ip.mi.dwFlags =  MOUSEEVENTF_LEFTDOWN;
			SendInput(1, &ip, sizeof(INPUT));
			//SendMessage(h_wnd_info.main_hWnd, WM_LBUTTONDOWN, MK_LBUTTON,
			//	MAKELPARAM(m_cursor_pos.x, m_cursor_pos.y));
			//SendMessage(h_wnd_info.main_hWnd, WM_LBUTTONUP, MK_LBUTTON,
			//	MAKELPARAM(m_cursor_pos.x, m_cursor_pos.y));
			ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			SendInput(1, &ip, sizeof(INPUT));
			set_state(fish_mgr::FISHING);
			Sleep(1000);
			init_fish();
			break;
		case FISHING:
			
			Sleep(30);
			m_fish_window.update_window();
			break;
		case COMPLETED:
			Sleep(2000);

			GetCursorPos(&m_cursor_pos);
			ip.type = INPUT_MOUSE;
			ip.mi.dwExtraInfo = GetMessageExtraInfo();
			ip.mi.mouseData = 0;
			ip.mi.time = 0;
			ip.mi.dx = m_cursor_pos.x;
			ip.mi.dy = m_cursor_pos.y;
			if (GetKeyState(VK_LBUTTON) & 0x80)
			{
				ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				SendInput(1, &ip, sizeof(INPUT));
			}
			ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			SendInput(1, &ip, sizeof(INPUT));
			//SendMessage(h_wnd_info.main_hWnd, WM_LBUTTONDOWN, MK_LBUTTON,
			//	MAKELPARAM(m_cursor_pos.x, m_cursor_pos.y));
			//SendMessage(h_wnd_info.main_hWnd, WM_LBUTTONUP, MK_LBUTTON,
			//	MAKELPARAM(m_cursor_pos.x, m_cursor_pos.y));
			ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			SendInput(1, &ip, sizeof(INPUT));
			break;
		}

	}
	m_state = UNDEFINED;

}


DWORD procs[1024];

hwnd_info h_wnd_info;
BOOL CALLBACK enum_proc_for_window(HWND hWnd, LPARAM lParam)
{
	DWORD dw_proc_id;
	DWORD dw_thread_id;
	dw_thread_id = GetWindowThreadProcessId(hWnd, &dw_proc_id);
	hwnd_info* p_wnd_info = (hwnd_info*)lParam;
	if (dw_proc_id == p_wnd_info->process_id && GetParent(hWnd) == NULL){
		p_wnd_info->main_hWnd = hWnd;
		p_wnd_info->main_thread_id = dw_thread_id;
		return FALSE;
	}
	return TRUE;
}
void get_main_window_hWnd(std::wstring name)
{
	DWORD procsnum = 0;
	TCHAR szEXEName[256] = { 0 };
	EnumProcesses(procs, 1024, &procsnum);
	for (int i = 0; i < procsnum; i++){
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
			procs[i]);
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
					h_wnd_info.process_id = procs[i];
					h_wnd_info.main_hWnd = NULL;
					CloseHandle(hProc);
					EnumWindows(enum_proc_for_window, (LPARAM)&h_wnd_info);
					break;
				}
				CloseHandle(hProc);
			}
		}
	}
}
void fish_window_process::process_fish_callback(const float **each_progress_width
	, const float *increased_progress_width,
	float progress)
{
	
	fwprintf(mem_set_log_file, L"leftborder: %.2f\t", increased_progress_width[2] + *each_progress_width[3] * 0.25);
	fwprintf(mem_set_log_file, L"progress: %.2f\t", progress);
	fwprintf(mem_set_log_file, L"rightborder: %.2f\n", increased_progress_width[3] - *each_progress_width[3] * 0.25);
	
	//fwprintf(mem_set_log_file, L"breakarea: %.2f\t", *each_progress_width[1]);
	//fwprintf(mem_set_log_file, L"break2success: %.2f\t", *each_progress_width[2]);
	//fwprintf(mem_set_log_file, L"success: %.2f\t", *each_progress_width[3]);
	//fwprintf(mem_set_log_file, L"success2miss: %.2f\t", *each_progress_width[4]);
	//fwprintf(mem_set_log_file, L"miss: %.2f\t", *each_progress_width[5]);
	//fwprintf(mem_set_log_file, L"rightboorder: %.2f\n", *each_progress_width[6]);
	INPUT ip;
	if (GetKeyState(VK_LBUTTON) & 0x80)
	{
		//fwprintf(mem_set_log_file, L"progress: %.2f\tleftborder: %.2f\n", progress, increased_progress_width[2] + *each_progress_width[3] * 0.1);
		if (progress < increased_progress_width[2] + *each_progress_width[3] * 0.25)
		{

			GetCursorPos(&m_cursor_pos);
			ip.type = INPUT_MOUSE;
			ip.mi.dwExtraInfo = GetMessageExtraInfo();
			ip.mi.mouseData = 0;
			ip.mi.time = 0;
			ip.mi.dx = m_cursor_pos.x;
			ip.mi.dy = m_cursor_pos.y;

			ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			SendInput(1, &ip, sizeof(INPUT));
		}
	}
	else
	{
		//fwprintf(mem_set_log_file, L"progress: %.2f\trightborder: %.2f\n", increased_progress_width[3] - *each_progress_width[3] * 0.1);
		if (progress > increased_progress_width[3] - *each_progress_width[3] * 0.25)
		{
			GetCursorPos(&m_cursor_pos);
			ip.type = INPUT_MOUSE;
			ip.mi.dwExtraInfo = GetMessageExtraInfo();
			ip.mi.mouseData = 0;
			ip.mi.time = 0;
			ip.mi.dx = m_cursor_pos.x;
			ip.mi.dy = m_cursor_pos.y;
			ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			SendInput(1, &ip, sizeof(INPUT));
		}
	}
}
