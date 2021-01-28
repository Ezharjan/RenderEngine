#pragma once
#include "LoadInAdvance.h"

namespace RenderEngine {

	class Window
	{
	public:
		static Window& getInstance()
		{
			static Window s_instance;
			return s_instance;
		}

		int Initialize(int windowWidth, int windowHeight, LPCTSTR windowTitle);
		void Update();
		void Dispatch();
		void Close();
		int IsKeyPressed(int key);
		int GetKeyUpEvent(int key);
		int shouldExit();
		LPVOID GetFrameBuffer();
		void ShowFPSInfo();


	private:
		// WndProc function from the globe must be static function
		static LRESULT CALLBACK WindowProcess(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		Window() {}; // constructor
		Window(const Window&) {}; // copy-construction
		Window& operator=(const Window& right) { return *this; } // assignment-construction
		~Window() {}; // destructor


		HWND wndHandle;
		HDC wndDC;
		HBITMAP wndHandleBMP;
		HBITMAP wndOldtBMP;
		LPVOID wndFramebuffer; // framebuffer
		int m_width;
		int m_height;
	};


	extern float gDistanceFactor;

}