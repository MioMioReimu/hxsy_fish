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
	
	//DirectInput hookԭ��
	//    DirectInput�� IDirectInput8��һ���ṹ�壬Ӧ��˵��һ����ָ�룬
	// ����ṹ��ĵ�һ��4�ֽ������ָ�룬����ڽ�����ȫ��Ψһ�ģ�
	//    �����ǵ���DirectInput8Createȥ��ȡ����ṹ������ָ��ʱ��ÿ�λ�ȡ���Ķ���ͬһ��ָ��
	//�����Ҵ���һ��m_lpInput����ʵ�Ҳ�������������Ҫ�޸����ȫ�ֵ����
	//�޸���֮��������֮ǰ����֮�󴴽���IDirectInput8�������˱仯����Ϊ����õĶ���ͬһ�����
	DWORD* m_lpInput_vtbl;
	//    IDIRECTINPUTDEVICE8Ҳ��ȫ��Ψһ�ģ����Կ��Բ�������ͬ���ķ�ʽ����hook
	DWORD* m_lpDevice_vtbl;
	// ����ṹ��� m_lpKeyָ�����ͬһ���ṹ��
	LPDIRECTINPUTDEVICE8 m_lpMouse;
};