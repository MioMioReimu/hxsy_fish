#pragma once
#include "stdafx.h"
class fish_window_callback
{
public:
	virtual void process_fish_callback(const float **each_progress_width, const float *increased_progress_width, float progress) = 0;
};
class fish_window
{
public:
	fish_window();
	bool set_window(void *pointer_window_name_ptr);
	fish_window_callback* set_callback(fish_window_callback * callback);
	bool update_window();
	bool init_window();
private:
	//fish_battle_window own pointer_window and progress_bar_windows;
	cegui_window * m_fish_battle_window;
	cegui_window * m_pointer_window;
	cegui_window*  m_progress_bar_windows[7];
	//7个从左到右 分别为LeftBorder 一直是4，BreakArea,Break2Success, 
	//SuccessArea, Success2Miss, MissArea,  RightBorder
	float *m_each_progress_bar_width[7];
	float  m_increased_progress_bar_width[7];
	Rect *m_pointer_rect;
	float  m_min_pointer_value; //(left_border_window.d_outerUnclippedRect.right-left)*0.5
	float m_max_pointer_value;//(right_border_window.d_outerUnclippedRect.right-left)*0.5
	fish_window_callback *m_callback;
};
class fish_window_process :public fish_window_callback
{
public:
	void process_fish_callback(const float **each_progress_width,
	const float *increased_progress_width, float progress);
private:
	POINT m_cursor_pos;
};
class fish_mgr
{
public:
	enum state
	{
		UNDEFINED,
		LOADED,
		INITED,
		FISHING,
		COMPLETED,
	};
	fish_mgr();
	//this function should be called by window_filter_process
	bool load_fish(void *pointer_window_name_ptr);
	bool init_fish();
	//开始钓鱼监控线程，
	bool start_fish();
	//关闭钓鱼监控线程
	bool stop_fish();
	//设置游戏中的状态 由draw_text_filter_process调用
	bool set_state(state fish_state);
	state get_state(){ return m_state; }
private:
	void fish_thread_proc();
	fish_window m_fish_window;
	//这个代表游戏中fish的状态
	state m_state;
	//钓鱼监控线程是否已经启动
	bool m_fish_thread_started;
	boost::mutex m_lock_thread_started;
	boost::thread * m_thread_handle;
	fish_window_callback * m_callback;
	POINT m_cursor_pos;
};