#include "Window.h"

#pragma region For showing FPS only
#include <timeapi.h>
#pragma comment(lib,"winmm.lib")
#pragma endregion


namespace RenderEngine {

	int keys[512]; // stores keys' state
	int releasedKeys[512];
	int exitSignal = 0;

	float gDistanceFactor = 3.f;
	Vector2 lastPointV2;
	Vector2 lastPointV2ForLight;


	// Achieve the user interaction related method here, using the given template (from ::WndProc)
	LRESULT CALLBACK Window::WindowProcess(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		//std::cout << "Message is " << message << std::endl;
		switch (message)
		{
		case WM_PAINT:
			ValidateRect(hwnd, NULL);
			break;

		case WM_RBUTTONDOWN:
			// right mouse button is clicked
			lastPointV2ForLight.setX(LOWORD(lParam));
			lastPointV2ForLight.setY(HIWORD(lParam));
			break;

		case WM_RBUTTONUP:
			break;

		case WM_LBUTTONDOWN:
			// left mouse button is clicked
	
			lastPointV2.setX(LOWORD(lParam));
			lastPointV2.setY(HIWORD(lParam));
			break;

		case WM_MOUSEMOVE:
			// Rotate the model on its world coordinate
			if (wParam & MK_LBUTTON)
			{
				// left button of the mouse pressed and moved
				Vector2 newPointV2(LOWORD(lParam), HIWORD(lParam));
				Device::getInstance().UpdateModelRotation(lastPointV2, newPointV2);
				lastPointV2 = newPointV2;
			}
			else if (wParam & MK_RBUTTON)
			{
				// right button of the mouse pressed and moved
				
				Device::getInstance().UpdateLightSpaceMatrix(); // Observer Pattern
				Device::getInstance().UpdateLightPosition(); // Observer Pattern

				float dx = DegreeToRad(0.25f * static_cast<float>(LOWORD(lParam) - lastPointV2ForLight.getX()));
				float dy = DegreeToRad(0.25f * static_cast<float>(HIWORD(lParam) - lastPointV2ForLight.getY()));

				Device::getInstance().theta -= dx;
				Device::getInstance().phi += dy;

				Device::getInstance().phi = Clamp(Device::getInstance().phi, 0.1f, MATH_PI - .1f);

				// update mouse position for light rotation
				lastPointV2ForLight.setX(LOWORD(lParam));
				lastPointV2ForLight.setY(HIWORD(lParam));
			}
			break;

		case WM_MOUSEWHEEL:
			if (wParam)
			{
				// get mouse wheel delta here
				//std::cout << LOWORD(wParam) << std::endl; // LOWORD: Low address of wParam, means ctrl or shift
				//std::cout << HIWORD(wParam) << std::endl; // HIWORD: High address of wParam, means scroll delta

				// Moving the camera to -Z
				if (HIWORD(wParam) % 120 == 0) // 120 is from Window-based API on mouse wheel delta
				{
					if (gDistanceFactor - EPSILON >= 0)
					{
						//std::cout << gDistanceFactor << std::endl;
						gDistanceFactor -= 0.05f;
					}
				}
				else if (HIWORD(wParam) == 0)
				{
					// do nothing
				}
				// Moving the camera to +Z
				else
				{
					if (gDistanceFactor <= 50) // magic number
					{
						//std::cout << gDistanceFactor << std::endl;
						gDistanceFactor += 0.05f;
					}
				}
			}
			break;

		case WM_KEYDOWN:
			//std::cout << wParam << std::endl;
			keys[wParam & 511] = 1;
			switch (wParam)
			{
			case VK_ESCAPE:
				exitSignal = 1;
				DestroyWindow(hwnd);
				PostQuitMessage(0);
				break;
			case VK_F1:
				break;
			case VK_F2:
				break;
			case VK_F3:
				break;
			case VK_UP:
				break;
			case VK_DOWN:
				break;
			case VK_RIGHT:
				break;
			case VK_LEFT:
				break;
			case VK_SPACE:
				break;
			case VK_F6:
				break;
			case VK_F7:
				break;
			default:
				break;
			}
			break;

		case WM_KEYUP:
			keys[wParam & 511] = 0;
			releasedKeys[wParam & 511] = 1;
			break;

		case WM_CLOSE:
			exitSignal = 1;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);

		}

		return 0;
	}

	int Window::Initialize(int windowWidth, int windowHeight, LPCTSTR windowTitle)
	{
		WNDCLASS wndClass = { CS_BYTEALIGNCLIENT, (WNDPROC)Window::WindowProcess, 0, 0, 0, NULL, NULL, NULL, NULL, "SCREEN" };
		// - windowHeight ==> top - down = 0 - windowHeight
		BITMAPINFO bitmapInfo = { {	sizeof(BITMAPINFOHEADER),windowWidth, -windowHeight, 1, 32,	BI_RGB,(DWORD)windowWidth * windowHeight * 4,0,0,0,0} };
		RECT rect = { 0, 0, windowWidth, windowHeight };

		this->Close();

		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.hInstance = GetModuleHandle(NULL);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		if (!RegisterClass(&wndClass)) return -1;

		wndHandle = CreateWindow("SCREEN", windowTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, 0, 0, NULL, NULL, wndClass.hInstance, NULL);
		if (wndHandle == NULL) return -2; // Means exception happened from the code above

		HDC hDC = GetDC(wndHandle);
		wndDC = CreateCompatibleDC(hDC);
		ReleaseDC(wndHandle, hDC);

		wndHandleBMP = CreateDIBSection(wndDC, &bitmapInfo, DIB_RGB_COLORS, &wndFramebuffer, 0, 0);
		if (wndHandleBMP == NULL) return -3;

		wndOldtBMP = (HBITMAP)SelectObject(wndDC, wndHandleBMP);
		this->m_width = windowWidth;
		this->m_height = windowHeight;

		AdjustWindowRect(&rect, GetWindowLong(wndHandle, GWL_STYLE), 0);
		int rectWidth = rect.right - rect.left;
		int rectHeight = rect.bottom - rect.top;
		//int wndPosX = (GetSystemMetrics(SM_CXSCREEN) - rectWidth) / 2;
		//int wndPosY = (GetSystemMetrics(SM_CYSCREEN) - rectHeight) / 2;
		//if (wndPosY < 0) wndPosY = 0;
		//SetWindowPos(wndHandle, NULL, wndPosX, wndPosY, rectWidth, rectHeight, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
		SetWindowPos(wndHandle, NULL, AT_SCREEN_POS_X, AT_SCREEN_POS_Y, rectWidth, rectHeight, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
		SetForegroundWindow(wndHandle);

		ShowWindow(wndHandle, SW_NORMAL);
		Dispatch();

		memset(wndFramebuffer, 0, windowWidth * windowHeight * 4);
		memset(keys, 0, sizeof(int) * 512);

		return 0;
	}

	void Window::Update()
	{
		HDC hDC = GetDC(wndHandle);
		BitBlt(hDC, 0, 0, m_width, m_height, wndDC, 0, 0, SRCCOPY);
		ReleaseDC(wndHandle, hDC);

		Dispatch();
	}

	void Window::Dispatch()
	{
		MSG msg{ 0 };
		while (!exitSignal) {
			if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			DispatchMessage(&msg);
		}
	}

	void Window::Close()
	{
		if (wndDC) {
			if (wndOldtBMP) {
				SelectObject(wndDC, wndOldtBMP);
				wndOldtBMP = NULL;
			}
			DeleteDC(wndDC);
			wndDC = NULL;
		}

		if (wndHandleBMP) {
			DeleteObject(wndHandleBMP);
			wndHandleBMP = NULL;
		}

		if (wndHandle) {
			CloseWindow(wndHandle);
			wndHandle = NULL;
		}
	}

	int Window::IsKeyPressed(int key)
	{
		return keys[key & 511];
	}

	int Window::GetKeyUpEvent(int key)
	{
		int result = releasedKeys[key];
		releasedKeys[key] = 0;
		return result;
	}

	int Window::shouldExit()
	{
		return exitSignal;
	}

	LPVOID Window::GetFrameBuffer()
	{
		return wndFramebuffer;
	}

	void Window::ShowFPSInfo()
	{
		static float  fps = 0;
		static int    frameCount = 0;
		static float  currentTime = 0.0f;
		static float  lastTime = 0.0f;

		HDC hDC = GetDC(wndHandle);
		frameCount++;
		currentTime = timeGetTime()*0.001f;

		if (currentTime - lastTime > 1.0f)
		{
			fps = (float)frameCount / (currentTime - lastTime);
			lastTime = currentTime;
			frameCount = 0;
		}
		SetTextColor(hDC, RGB(255, 255, 255));
		SetBkMode(hDC, TRANSPARENT);
		char strFPS[20];
		sprintf_s(strFPS, 20, "FPS:%f", fps);
		TextOutA(hDC, 0, 0, strFPS, 10);

		// Other infos about interation introduction
		char strInfo[100];
		sprintf_s(strInfo, 100, "Press F1-F6 to see the different mode.");
		TextOutA(hDC, 0, 580, strInfo, 38); // there are 38 words in the sentence above, if the size is large, a C/ character will be shown.


		ReleaseDC(wndHandle, hDC); // in case of flashing
	}


}
