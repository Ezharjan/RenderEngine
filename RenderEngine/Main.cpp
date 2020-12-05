#include "PipelineCore.h"



void NULLFUNC(short x,short y, short z) {}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef _DEBUG
	DesktopCPPDebugger::UseCosoleBoard();
#endif // DEBUG_MODE
	

	WNDCLASSEX wndclass = { 0 };
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = (HICON)::LoadImage(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "Renderer";

	if (!RegisterClassEx(&wndclass))
		return -1;


	HWND hwnd;
	//hwnd = CreateWindow("Renderer", WINDOW_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);
	// The code belowban user from resizing the window
	hwnd = CreateWindow("Renderer", WINDOW_TITLE, WS_OVERLAPPEDWINDOW^WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	RenderEngine::Initlize3D(hwnd);

	MoveWindow(hwnd, AT_SCREEN_POS_X, AT_SCREEN_POS_Y, RenderEngine::windowWidth, RenderEngine::windowHeight, true);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	//RenderEngine::UpdateScene(RenderEngine::RotateOBJByUser, 0, 0);//提前初始化一下整个场景，以免出现只有交互的时候才开始绘制的情况

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			RenderEngine::ClearBackBuffer(BGColor_R, BGColor_G, BGColor_B);				

			RenderEngine::ClearDepthBuffer(1.0f);

			RenderEngine::UpdateScene(NULLFUNC,1,1,1); //使用此更新方式时不接受外部交互的输入，因此在有交互的情况下不要在这里更新场景数据，但是记得初始化一下场景
			
			RenderEngine::Render(RenderEngine::MyDisplayMode);
		}
	}

	UnregisterClass("Renderer", wndclass.hInstance);
	return 0;
}






RenderEngine::Vector2 lastPointV2;

static float indexTmp = 88.f;

// Achieve the user interaction related method here, using the given template
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static LPARAM lpCur = 0;

	//std::cout << "Message is " << message << std::endl;
	switch (message)
	{
	case WM_PAINT:
		ValidateRect(hwnd, NULL);
		break;

	case WM_RBUTTONDOWN: //---123：Right button
		
		lastPointV2.x = LOWORD(lParam);
		lastPointV2.y = HIWORD(lParam);
		
	



		//// FOR TEST ONLY
		//lastPointV2.x = 400;
		//lastPointV2.y = indexTmp;

		//RenderEngine::gLastRotation = RenderEngine::gCurrentRotation;
		//RenderEngine::ArcBallTransform(lastPointV2, RenderEngine::gLastMousePointV3);
			   
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			// left button of the mouse pressed and moved
		}
		else if (wParam & MK_RBUTTON)
		{

			RenderEngine::Vector2 newPointV2;
			newPointV2.x = LOWORD(lParam);
			newPointV2.y = HIWORD(lParam);

	
			RenderEngine::UpdateViewMatrix4Rotation(lastPointV2,newPointV2);

			lastPointV2 = newPointV2;
			
			std::cout << "cam pos X -: " << RenderEngine::CameraPos.getX() << std::endl;
			std::cout << "cam pos Y -: " << RenderEngine::CameraPos.getY() << std::endl;
			std::cout << "cam pos Z -: " << RenderEngine::CameraPos.getZ() << std::endl;
			std::cout << "\n " << std::endl;
		}
		break;

	case WM_MOUSEWHEEL:
		if (wParam)
		{
			std::cout << GET_WHEEL_DELTA_WPARAM(wParam) << std::endl; // Unit: 120 steps/angle
			RenderEngine::UpdateScene(RenderEngine::ScaleOBJByUser, 0, 0, -GET_WHEEL_DELTA_WPARAM(wParam));
		}
		break;

	case WM_KEYDOWN:
		// 按下Esc键时毁掉窗口
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
		}
		else if (wParam == VK_F1)
		{
			RenderEngine::MyDisplayMode = RenderEngine::WIRE_FRAME_MODE;
			std::cout << "Done 1" << std::endl;
		}
		else if (wParam == VK_F2)
		{
			RenderEngine::MyDisplayMode = RenderEngine::SOLID_MODE;
			std::cout << "Done 2" << std::endl;
		}
		else if (wParam == VK_F3)
		{
			RenderEngine::MyDisplayMode = RenderEngine::TEXTURE_MODE;
			std::cout << "Done 3" << std::endl;
		}
		else if (wParam == VK_UP)
		{
			RenderEngine::MoveCameraPosition(RenderEngine::goUp);
		}
		else if (wParam == VK_DOWN)
		{
			RenderEngine::MoveCameraPosition(RenderEngine::goDown);
		}
		else if (wParam == VK_RIGHT)
		{
			RenderEngine::MoveCameraPosition(RenderEngine::goRight);
		}
		else if (wParam == VK_LEFT)
		{
			RenderEngine::MoveCameraPosition(RenderEngine::goLeft);
		}
		else if (wParam == VK_F5) 
		{
			printf("%s", "down button clicked\n");
			RenderEngine::MoveCameraPosition(RenderEngine::goIn);
		}
		else if (wParam == VK_F6)
		{
			printf("%s", "down button clicked\n");
			RenderEngine::MoveCameraPosition(RenderEngine::goOut);
		}
		else if (wParam == VK_F7) {}
		else if (wParam == VK_F8) {}
		else if (wParam == VK_SPACE) 
		{
			//// FOR TEST ONLY!!!

			//newPointV2.x = 400;

			//if (indexTmp < 500)
			//{
			//	indexTmp+=1;
			//}
			//else
			//{
			//	indexTmp = 0;
			//}
			//newPointV2.y = indexTmp;

			//std::cout << "indexTemp: " << indexTmp << std::endl;

			//RenderEngine::UpdateViewMatrix4Rotation(newPointV2);
			//std::cout << "cam pos X : " << RenderEngine::CameraPos.getX() << std::endl;
			//std::cout << "cam pos Y : " << RenderEngine::CameraPos.getY() << std::endl;
			//std::cout << "cam pos Z : " << RenderEngine::CameraPos.getZ() << std::endl;
			//std::cout << "\n " << std::endl;
		}
		break;

	case WM_KEYUP:
		// 下方一定要加上跳出语句！！！
		break;

	case WM_DESTROY:
		RenderEngine::Release3D(); //释放资源
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
