#pragma once
#include "stdafx.h"
#include <dinput.h>
class dInput_Hook
{
public:
	enum HOOK_FUNC
	{
		Hook_DInput_CreateDevice = 0x1000000C,
		Hook_DInput_EnumDevices = 0x10000010,
		Hook_DInput_GetDeviceStatus = 0x10000014,
		Hook_DInput_RunControlPanel = 0x10000018,
		Hook_DInput_Initialize = 0x1000001C,
		Hook_DInput_FindDevice = 0x10000020,
		Hook_DInput_EnumDeviceBySemantics = 0x10000024,
		Hook_DInput_ConfigureDevices = 0x10000028,

		Hook_DIDevice_GetCapabilities = 0x0000000C,
		Hook_DIDevice_EnumObjects = 0x00000010,
		Hook_DIDevice_GetProperty = 0x00000014,
		Hook_DIDevice_SetProperty = 0x00000018,
		Hook_DIDevice_Acquire = 0x0000002C,
		Hook_DIDevice_Unacquire = 0x00000030,
		Hook_DIDevice_GetDeviceState = 0x00000034,
		Hook_DIDevice_GetDeviceData = 0x00000038,
		Hook_DIDevice_SetDataFormat = 0x0000003C,
		Hook_DIDevice_SetEventNotification = 0x00000040,
		Hook_DIDevice_SetCooperativeLevel = 0x00000044,
		Hook_DIDevice_GetObjectInfo = 0x00000048,
		Hook_DIDevice_GetDeviceInfo = 0x0000004C,
		Hook_DIDevice_RunControlPanel = 0x00000050,
		Hook_DIDevice_Initialize = 0x00000054,
		Hook_DIDevice_CreateEffect = 0x00000058,
		Hook_DIDevice_EnumEffects = 0x0000005C,
		Hook_DIDevice_GetEffectInfo = 0x00000060,
		Hook_DIDevice_GetForceFeedbackState = 0x00000064,
		Hook_DIDevice_SendForceFeedbackCommand = 0x00000068,
		Hook_DIDevice_EnumCreatedEffectObjects = 0x0000006C,
		Hook_DIDevice_Escape = 0x00000070,
		Hook_DIDevice_Poll = 0x00000074,
		Hook_DIDevice_SendDeviceData = 0x00000078,
		Hook_DIDevice_EnumEffectsInFile = 0x0000007C,
		Hook_DIDevice_WriteEffectToFile = 0x00000080,
		Hook_DIDevice_BuildActionMap = 0x00000084,
		Hook_DIDevice_SetActionMap = 0x00000088,
		Hook_DIDevice_GetImageInfo = 0x0000008C,
	};
	void hook_dinput_func(enum  HOOK_FUNC type, void *fun_ptr);
private:
	//
	HRESULT create_input_device();
	
	//DirectInput hook原理
	//    DirectInput中 IDirectInput8是一个结构体，应该说是一个类指针，
	// 这个结构体的第一个4字节是虚表指针，虚表在进程是全局唯一的，
	//    当我们调用DirectInput8Create去获取这个结构体的虚表指针时，每次获取到的都是同一个指针
	//所以我创建一个m_lpInput，其实我不会用它，我是要修改这个全局的虚表。
	//修改了之后，其他的之前或者之后创建的IDirectInput8都发生了变化，因为大家用的都是同一个虚表。
	DWORD* m_lpInput_vtbl;
	//    IDIRECTINPUTDEVICE8也是全局唯一的，所以可以采用上面同样的方式进行hook
	DWORD* m_lpDevice_vtbl;
	// 这个结构体和 m_lpKey指向的是同一个结构体
	LPDIRECTINPUTDEVICE8 m_lpMouse;
};