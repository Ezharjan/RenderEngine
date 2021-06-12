#include "vector_t.h"
#include <iostream>
#include<windows.h>
#include <tchar.h>
#include "device_t.h"
#include "logic.h"

using namespace std;
typedef unsigned int IUINT32;
#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#endif
//=====================================================================
// 数学库：此部分应该不用详解，熟悉 D3D 矩阵变换即可
//=====================================================================

//=====================================================================
// Win32 窗口及图形绘制：为 device 提供一个 DibSection 的 FB
//=====================================================================
int screen_w, screen_h, screen_exit = 0;
int screen_mx = 0, screen_my = 0, screen_mb = 0;
int screen_keys[512];	// 当前键盘按下状态
static HWND screen_handle = NULL;		// 主窗口 HWND
static HDC screen_dc = NULL;			// 配套的 HDC
static HBITMAP screen_hb = NULL;		// DIB
static HBITMAP screen_ob = NULL;		// 老的 BITMAP
unsigned char *screen_fb = NULL;		// frame buffer
long screen_pitch = 0;

int screen_init(int w, int h, const TCHAR *title);	// 屏幕初始化
int screen_close(void);								// 关闭屏幕
void screen_dispatch(void);							// 处理消息
void screen_update(void);							// 显示 FrameBuffer

													// win32 event handler
static LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);



// 初始化窗口并设置标题，参考《3D游戏编程大师技巧》第二章
int screen_init(int w, int h, const TCHAR *title) {
	//1.创建windows类。利用WNDCLASS保存windows类信息。
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
		NULL, NULL, NULL, NULL, _T("SCREEN3.1415926") };//WNDCLASS   窗口类
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
		w * h * 4, 0, 0, 0, 0 } };//BITMAPINFO 该结构定义了DIB（设备无关位图）的大小和颜色信息
	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;//HDC（设备描述表句柄）。WINDOWS程序中并不是用物理地址来标识一个内存块，文件，任务或动态装入模块的，相反的，WINDOWS   API给这些项目分配确定的句柄，并将句柄返回给应用程序，然后通过句柄来进行操作。   

	screen_close();

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//GetStockObject 函数用于获得 Windows 预设的画笔、画刷、字体或者调色板的句柄。

	wc.hInstance = GetModuleHandle(NULL);//GetModuleHandle   获取指定模块的句柄
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//LoadCursor 载入鼠标。当模块句柄为空时，LoadCursor则从系统中获取光标。
											 //2.注册windows类。定义windows类并将其存储到winclass中后，还必须通过注册让windows操作系统知道这个类。
	if (!RegisterClass(&wc)) return -1;//RegisterClass就函数名字和其参数来解释，中文意思是注册窗口类别，顾名思义就是注册一个窗口类
									   //3.创建窗口通过CreateWindow（）。
	screen_handle = CreateWindow(_T("SCREEN3.1415926"), title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);//CreateWindow字面上意思就是创建一个窗口。RegisterClass函数的作用是通知系统，你要定义一个新的窗体类型，然后把这个类型记录到系统里面，以后你就可以使用CreateWindow来创建一个基于此类型的窗体。基于此类型的窗体都具有相同的属性，比如，背景色，光标，图标等等。
	if (screen_handle == NULL) return -2;

	screen_exit = 0;
	hDC = GetDC(screen_handle);//GetDC函数根据指定的等级类型检索指定窗口普通的、典型的或特有的设备上下文环境。
	screen_dc = CreateCompatibleDC(hDC);//在窗口的DC之外，可以建立自己的DC，就是说它不对应窗口，这个方法就是CreateCompatibleDC，这个DC就是一个内存缓冲区，通过这个DC你可以把和它兼容的窗口DC保存到这个DC中，就是说你可以通过它在不同的DC之间拷贝数据。例如：你先在这个DC中建立好数据，然后在拷贝到窗口的DC就是完成了这个窗口的刷新。https://blog.csdn.net/shellching/article/details/18405185
	ReleaseDC(screen_handle, hDC);//释放句柄

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);//screen_dc设备环境句柄；&bi指向BITMAPINFO结构的指针；DIB_RGB_COLORS指定由pbmi参数指定的BITMAPINFO结构中的成员bmiColors数组包含的数据类型（要么是逻辑调色板索引值，要么是原文的RGB值）；&ptr指向一个变量的指针，该变量接收一个指向DIB位数据值的指针；0文件映射对象的句柄。函数将使用该对象来创建DIB（与设备无关位图）。该参数可以是NULL。；0指定从hSection引用的文件映射对象开始处算起的偏移量，超过这个偏移量的地方就是位图的位数据值开始存放的地方。在hSection为NULL时忽略该值。位图的位数据值是以DWORD为单位计算的。；返回值：如果函数执行成功，那么返回值是一个指向刚刚创建的与设备无关位图的句柄，并且&ptr指向该位图的位数据值；如果函数执行失败，那么返回值为NULL，并且*ppvBit也为NULL，若想获得更多错误信息，请调用GetLastError函数。https://baike.baidu.com/item/CreateDIBSection/6377219


	if (screen_hb == NULL) return -3;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);//SelectObject的作用是将指定GDI对象选定进入指定的DC,并作为其绘画时使用的当前对象。
	screen_fb = (unsigned char*)ptr;//数据存入帧缓存
	screen_w = w;//屏幕宽度
	screen_h = h;//屏幕高度
	screen_pitch = w * 4;//此变量没有用到，注释之后不影响程序正常运行

	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);//AdjustWindowRect依据所需客户矩形的大小,计算需要的窗口矩形的大小
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;//GetSystemMetrics(SM_CXSCREEN)获取以像素为单位的屏幕尺寸。https://baike.baidu.com/item/GetSystemMetrics/5608817s
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;//同上
	if (sy < 0) sy = 0;
	SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));//SetWindowPos函数改变一个子窗口，弹出式窗口或顶层窗口的尺寸，位置和Z序。子窗口，弹出式窗口，及顶层窗口根据它们在屏幕上出现的顺序排序、顶层窗口设置的级别最高，并且被设置为Z序的第一个窗口。参数详情https://baike.baidu.com/item/SetWindowPos/6376849?fr=aladdin
																										//SWP_NOCOPYBITS：清除客户区的所有内容。SWP_NOZORDER：维持当前Z序（忽略hWndlnsertAfter参数）。SWP_SHOWWINDOW：显示窗口。


	SetForegroundWindow(screen_handle);//函数功能：该函数将创建指定窗口的线程设置到前台，并且激活该窗口。键盘输入转向该窗口，并为用户改各种可视的记号。系统给创建前台窗口的线程分配的权限稍高于其他线程。

	ShowWindow(screen_handle, SW_NORMAL);//显示窗口
	screen_dispatch();//处理消息

	memset(screen_keys, 0, sizeof(int) * 512);//初始化screen_keys
	memset(screen_fb, 0, w * h * 4);//初始化screen_fb

	return 0;
}

int screen_close(void) {
	if (screen_dc) {
		if (screen_ob) {
			SelectObject(screen_dc, screen_ob);
			screen_ob = NULL;
		}
		DeleteDC(screen_dc);
		screen_dc = NULL;
	}
	if (screen_hb) {
		DeleteObject(screen_hb);
		screen_hb = NULL;
	}
	if (screen_handle) {
		CloseWindow(screen_handle);
		screen_handle = NULL;
	}
	return 0;
}

//窗口事件的回调函数，用来处理键盘输入事件
static LRESULT screen_events(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE: screen_exit = 1; break;
	case WM_KEYDOWN: screen_keys[wParam & 511] = 1; break;
	case WM_KEYUP: screen_keys[wParam & 511] = 0; break;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

//处理消息
void screen_dispatch(void) {
	MSG msg;
	while (1) {
		//使用PeekMessage()和PM_NOREMOVE标记，如果消息队列中有消息，则调用GetMessage()函数。
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;//PeekMessage是一个Windows API函数。该函数为一个消息检查线程消息队列，并将该消息（如果存在）放于指定的结构。 https://baike.baidu.com/item/PeekMessage
		if (!GetMessage(&msg, NULL, 0, 0)) break;//GetMessage是从调用线程的消息队列里取得一个消息并将其放于指定的结构。https://baike.baidu.com/item/GetMessage
		DispatchMessage(&msg);//DispatchMessage函数功能：该函数分发一个消息给窗口程序（window proc）。通常消息从GetMessage函数获得或者TranslateMessage函数传递的。消息被分发到回调函数（过程函数)，作用是消息传递给操作系统，然后操作系统去调用我们的回调函数，也就是说我们在窗体的过程函数中处理消息。
	}
}

//屏幕更新
void screen_update(void) {
	HDC hDC = GetDC(screen_handle);
	BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(screen_handle, hDC);
	screen_dispatch();//处理消息
}




int main()
{
	device_t device;
	int states[] = { RENDER_STATE_TEXTURE, RENDER_STATE_COLOR, RENDER_STATE_WIREFRAME };
	int indicator = 0;
	int kbhit = 0;
	float alpha = 1;
	float pos = 3.5;

	TCHAR *title = _T("Mini3d (software render tutorial) - ")
		_T("Left/Right: rotation, Up/Down: forward/backward, Space: switch state");

	if (screen_init(800, 600, title)) //初始化窗口并设定标题
		return -1;

	device.device_init(800, 600, screen_fb);//初始化设备
	logic _logic;
	_logic.camera_at_zero(device, 3, 0, 0);//初始化相机

	_logic.init_texture(device);//初始化纹理
	device.render_state = RENDER_STATE_TEXTURE;//设置渲染模式

	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0)
	{
		screen_dispatch();//处理消息
		device.device_clear(1);// 清空 framebuffer 和 zbuffer
		_logic.camera_at_zero(device, pos, 0, 0);//更新相机参数
		//处理键盘按下
		if (screen_keys[VK_UP]) pos -= 0.01f;
		if (screen_keys[VK_DOWN]) pos += 0.01f;
		if (screen_keys[VK_LEFT]) alpha += 0.01f;
		if (screen_keys[VK_RIGHT]) alpha -= 0.01f;
		if (screen_keys[VK_SPACE]) {
			if (kbhit == 0) {
				kbhit = 1;
				if (++indicator >= 3) indicator = 0;
				device.render_state = states[indicator];
			}
		}
		else {
			kbhit = 0;
		}
		_logic.draw_box(device, alpha);//绘制六面体
		screen_update();//屏幕更新
		Sleep(1);

	}

	return 0;
}