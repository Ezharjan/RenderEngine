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
// ��ѧ�⣺�˲���Ӧ�ò�����⣬��Ϥ D3D ����任����
//=====================================================================

//=====================================================================
// Win32 ���ڼ�ͼ�λ��ƣ�Ϊ device �ṩһ�� DibSection �� FB
//=====================================================================
int screen_w, screen_h, screen_exit = 0;
int screen_mx = 0, screen_my = 0, screen_mb = 0;
int screen_keys[512];	// ��ǰ���̰���״̬
static HWND screen_handle = NULL;		// ������ HWND
static HDC screen_dc = NULL;			// ���׵� HDC
static HBITMAP screen_hb = NULL;		// DIB
static HBITMAP screen_ob = NULL;		// �ϵ� BITMAP
unsigned char *screen_fb = NULL;		// frame buffer
long screen_pitch = 0;

int screen_init(int w, int h, const TCHAR *title);	// ��Ļ��ʼ��
int screen_close(void);								// �ر���Ļ
void screen_dispatch(void);							// ������Ϣ
void screen_update(void);							// ��ʾ FrameBuffer

													// win32 event handler
static LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);



// ��ʼ�����ڲ����ñ��⣬�ο���3D��Ϸ��̴�ʦ���ɡ��ڶ���
int screen_init(int w, int h, const TCHAR *title) {
	//1.����windows�ࡣ����WNDCLASS����windows����Ϣ��
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
		NULL, NULL, NULL, NULL, _T("SCREEN3.1415926") };//WNDCLASS   ������
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
		w * h * 4, 0, 0, 0, 0 } };//BITMAPINFO �ýṹ������DIB���豸�޹�λͼ���Ĵ�С����ɫ��Ϣ
	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;//HDC���豸������������WINDOWS�����в������������ַ����ʶһ���ڴ�飬�ļ��������̬װ��ģ��ģ��෴�ģ�WINDOWS   API����Щ��Ŀ����ȷ���ľ��������������ظ�Ӧ�ó���Ȼ��ͨ����������в�����   

	screen_close();

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//GetStockObject �������ڻ�� Windows Ԥ��Ļ��ʡ���ˢ��������ߵ�ɫ��ľ����

	wc.hInstance = GetModuleHandle(NULL);//GetModuleHandle   ��ȡָ��ģ��ľ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//LoadCursor ������ꡣ��ģ����Ϊ��ʱ��LoadCursor���ϵͳ�л�ȡ��ꡣ
											 //2.ע��windows�ࡣ����windows�ಢ����洢��winclass�к󣬻�����ͨ��ע����windows����ϵͳ֪������ࡣ
	if (!RegisterClass(&wc)) return -1;//RegisterClass�ͺ������ֺ�����������ͣ�������˼��ע�ᴰ����𣬹���˼�����ע��һ��������
									   //3.��������ͨ��CreateWindow������
	screen_handle = CreateWindow(_T("SCREEN3.1415926"), title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);//CreateWindow��������˼���Ǵ���һ�����ڡ�RegisterClass������������֪ͨϵͳ����Ҫ����һ���µĴ������ͣ�Ȼ���������ͼ�¼��ϵͳ���棬�Ժ���Ϳ���ʹ��CreateWindow������һ�����ڴ����͵Ĵ��塣���ڴ����͵Ĵ��嶼������ͬ�����ԣ����磬����ɫ����꣬ͼ��ȵȡ�
	if (screen_handle == NULL) return -2;

	screen_exit = 0;
	hDC = GetDC(screen_handle);//GetDC��������ָ���ĵȼ����ͼ���ָ��������ͨ�ġ����͵Ļ����е��豸�����Ļ�����
	screen_dc = CreateCompatibleDC(hDC);//�ڴ��ڵ�DC֮�⣬���Խ����Լ���DC������˵������Ӧ���ڣ������������CreateCompatibleDC�����DC����һ���ڴ滺������ͨ�����DC����԰Ѻ������ݵĴ���DC���浽���DC�У�����˵�����ͨ�����ڲ�ͬ��DC֮�俽�����ݡ����磺���������DC�н��������ݣ�Ȼ���ڿ��������ڵ�DC���������������ڵ�ˢ�¡�https://blog.csdn.net/shellching/article/details/18405185
	ReleaseDC(screen_handle, hDC);//�ͷž��

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);//screen_dc�豸���������&biָ��BITMAPINFO�ṹ��ָ�룻DIB_RGB_COLORSָ����pbmi����ָ����BITMAPINFO�ṹ�еĳ�ԱbmiColors����������������ͣ�Ҫô���߼���ɫ������ֵ��Ҫô��ԭ�ĵ�RGBֵ����&ptrָ��һ��������ָ�룬�ñ�������һ��ָ��DIBλ����ֵ��ָ�룻0�ļ�ӳ�����ľ����������ʹ�øö���������DIB�����豸�޹�λͼ�����ò���������NULL����0ָ����hSection���õ��ļ�ӳ�����ʼ�������ƫ�������������ƫ�����ĵط�����λͼ��λ����ֵ��ʼ��ŵĵط�����hSectionΪNULLʱ���Ը�ֵ��λͼ��λ����ֵ����DWORDΪ��λ����ġ�������ֵ���������ִ�гɹ�����ô����ֵ��һ��ָ��ոմ��������豸�޹�λͼ�ľ��������&ptrָ���λͼ��λ����ֵ���������ִ��ʧ�ܣ���ô����ֵΪNULL������*ppvBitҲΪNULL�������ø��������Ϣ�������GetLastError������https://baike.baidu.com/item/CreateDIBSection/6377219


	if (screen_hb == NULL) return -3;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);//SelectObject�������ǽ�ָ��GDI����ѡ������ָ����DC,����Ϊ��滭ʱʹ�õĵ�ǰ����
	screen_fb = (unsigned char*)ptr;//���ݴ���֡����
	screen_w = w;//��Ļ���
	screen_h = h;//��Ļ�߶�
	screen_pitch = w * 4;//�˱���û���õ���ע��֮��Ӱ�������������

	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);//AdjustWindowRect��������ͻ����εĴ�С,������Ҫ�Ĵ��ھ��εĴ�С
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;//GetSystemMetrics(SM_CXSCREEN)��ȡ������Ϊ��λ����Ļ�ߴ硣https://baike.baidu.com/item/GetSystemMetrics/5608817s
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;//ͬ��
	if (sy < 0) sy = 0;
	SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));//SetWindowPos�����ı�һ���Ӵ��ڣ�����ʽ���ڻ򶥲㴰�ڵĳߴ磬λ�ú�Z���Ӵ��ڣ�����ʽ���ڣ������㴰�ڸ�����������Ļ�ϳ��ֵ�˳�����򡢶��㴰�����õļ�����ߣ����ұ�����ΪZ��ĵ�һ�����ڡ���������https://baike.baidu.com/item/SetWindowPos/6376849?fr=aladdin
																										//SWP_NOCOPYBITS������ͻ������������ݡ�SWP_NOZORDER��ά�ֵ�ǰZ�򣨺���hWndlnsertAfter��������SWP_SHOWWINDOW����ʾ���ڡ�


	SetForegroundWindow(screen_handle);//�������ܣ��ú���������ָ�����ڵ��߳����õ�ǰ̨�����Ҽ���ô��ڡ���������ת��ô��ڣ���Ϊ�û��ĸ��ֿ��ӵļǺš�ϵͳ������ǰ̨���ڵ��̷߳����Ȩ���Ը��������̡߳�

	ShowWindow(screen_handle, SW_NORMAL);//��ʾ����
	screen_dispatch();//������Ϣ

	memset(screen_keys, 0, sizeof(int) * 512);//��ʼ��screen_keys
	memset(screen_fb, 0, w * h * 4);//��ʼ��screen_fb

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

//�����¼��Ļص�����������������������¼�
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

//������Ϣ
void screen_dispatch(void) {
	MSG msg;
	while (1) {
		//ʹ��PeekMessage()��PM_NOREMOVE��ǣ������Ϣ����������Ϣ�������GetMessage()������
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;//PeekMessage��һ��Windows API�������ú���Ϊһ����Ϣ����߳���Ϣ���У���������Ϣ��������ڣ�����ָ���Ľṹ�� https://baike.baidu.com/item/PeekMessage
		if (!GetMessage(&msg, NULL, 0, 0)) break;//GetMessage�Ǵӵ����̵߳���Ϣ������ȡ��һ����Ϣ���������ָ���Ľṹ��https://baike.baidu.com/item/GetMessage
		DispatchMessage(&msg);//DispatchMessage�������ܣ��ú����ַ�һ����Ϣ�����ڳ���window proc����ͨ����Ϣ��GetMessage������û���TranslateMessage�������ݵġ���Ϣ���ַ����ص����������̺���)����������Ϣ���ݸ�����ϵͳ��Ȼ�����ϵͳȥ�������ǵĻص�������Ҳ����˵�����ڴ���Ĺ��̺����д�����Ϣ��
	}
}

//��Ļ����
void screen_update(void) {
	HDC hDC = GetDC(screen_handle);
	BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(screen_handle, hDC);
	screen_dispatch();//������Ϣ
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

	if (screen_init(800, 600, title)) //��ʼ�����ڲ��趨����
		return -1;

	device.device_init(800, 600, screen_fb);//��ʼ���豸
	logic _logic;
	_logic.camera_at_zero(device, 3, 0, 0);//��ʼ�����

	_logic.init_texture(device);//��ʼ������
	device.render_state = RENDER_STATE_TEXTURE;//������Ⱦģʽ

	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0)
	{
		screen_dispatch();//������Ϣ
		device.device_clear(1);// ��� framebuffer �� zbuffer
		_logic.camera_at_zero(device, pos, 0, 0);//�����������
		//������̰���
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
		_logic.draw_box(device, alpha);//����������
		screen_update();//��Ļ����
		Sleep(1);

	}

	return 0;
}