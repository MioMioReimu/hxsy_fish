#pragma once
struct Rect
{
	float d_left, d_top,d_right,d_bottom;
};
enum HorizontalAlignment
{
	/**
	* Window's position specifies an offset of it's left edge from the left
	* edge of it's parent.
	*/
	HA_LEFT,
	/**
	* Window's position specifies an offset of it's horizontal centre from the
	* horizontal centre of it's parent.
	*/
	HA_CENTRE,
	/**
	* Window's position specifies an offset of it's right edge from the right
	* edge of it's parent.
	*/
	HA_RIGHT
};
enum VerticalAlignment
{
	/**
	* Window's position specifies an offset of it's top edge from the top edge
	* of it's parent.
	*/
	VA_TOP,
	/**
	* Window's position specifies an offset of it's vertical centre from the
	* vertical centre of it's parent.
	*/
	VA_CENTRE,
	/**
	* Window's position specifies an offset of it's bottom edge from the
	* bottom edge of it's parent.
	*/
	VA_BOTTOM
};

struct cegui_window
{
	unsigned char not_concern_0[108];
	union{
		//当name_str第一个wchar_t为0x0000时 表示这个字符串过长，以外部指针的方式链接到字符串。
		wchar_t name_str[40];
		struct {
			unsigned short not_used_prefix[32];
			//外部链接字符串
			wchar_t *name__str_ptr;
			unsigned short not_used[6];
		};
	} name;
	struct{
		void*  not_concern_0;
		cegui_window ** list_ptr;
		void*  not_concern_1[2];
	} childs;	//子窗口
	struct{
		void*  not_concern_0;
		cegui_window * list_ptr;
		void*  not_concern_1[2];
	} rendered_childs;//需要被渲染的子窗口
	cegui_window * parent;//父窗口

	unsigned char not_conern_1[456];
	float d_area[8];
	float d_pixelSize[2];//very concern; [0]:x pixels, [1]: y pixels;
	float d_minSize[4];// not important
	float d_maxSize[4];//not important
	HorizontalAlignment d_horzAlign;
	VerticalAlignment d_vertAlign;
	float d_rotation[3];
	//! outer area rect in screen pixels
	mutable Rect d_outerUnclippedRect;
	//! inner area rect in screen pixels
	mutable Rect d_innerUnclippedRect;
	//! outer area clipping rect in screen pixels
	mutable Rect d_outerRectClipper;
	//! inner area clipping rect in screen pixels
	mutable Rect d_innerRectClipper;
	//! area rect used for hit-testing agains this window
	mutable Rect d_hitTestRect;

	mutable bool d_outerUnclippedRectValid;
	mutable bool d_innerUnclippedRectValid;
	mutable bool d_outerRectClipperValid;
	mutable bool d_innerRectClipperValid;
	mutable bool d_hitTestRectValid;
};
