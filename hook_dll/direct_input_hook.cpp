#pragma once
#include "stdafx.h"
#include "direct_input_hook.h"
#include "hook_dll.h"
HRESULT dInput_Hook::create_input_device()
{
	LPDIRECTINPUT8 lpInput;
	LPDIRECTINPUTDEVICE8 lpDevice;
	HRESULT hr;
	if ((hr=DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&lpInput, NULL)) != DI_OK)
		return hr;
	if ((hr = lpInput->CreateDevice(GUID_SysKeyboard, &lpDevice, NULL)) != DI_OK)
		return hr;
	if ((hr = lpDevice->SetCooperativeLevel(h_wnd_info.main_hWnd, DISCL_BACKGROUND
		| DISCL_NONEXCLUSIVE)) != DI_OK)
		return hr;
	if ((hr = lpDevice->SetDataFormat(&c_dfDIKeyboard)) != DI_OK)
		return hr;
	if ((hr = lpDevice->Acquire() != DI_OK))
		return hr;
	m_lpInput_vtbl = (DWORD*)(*((DWORD*)lpInput));
	m_lpDevice_vtbl = (DWORD*)*lpDevice;
}
void hook_dinput_func(enum  HOOK_FUNC type, void *fun_ptr)
{
	DWORD flOldProtect, flNewProtect, flDontCare;
	MEMORY_BASIC_INFORMATION mbi;
	if ((type & 0xFFFF0000) == 0x10000000)
	{
		// Get the current protection attributes of the vtable                           
		VirtualQuery((void*)pvtbl, &mbi, sizeof(mbi));
		// remove ReadOnly and ExecuteRead attributes, add on ReadWrite flag
		flNewProtect = mbi.Protect;
		flNewProtect &= ~(PAGE_READONLY | PAGE_EXECUTE_READ | PAGE_EXECUTE);
		flNewProtect |= (PAGE_READWRITE);
	}
	else if ((type & 0xFFFF0000) == 0x0)
	{

	}
}