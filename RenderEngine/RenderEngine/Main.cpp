#include "LoadInAdvance.h"

#if _DEBUG
#include "DebugHelper.h"
#endif


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef _DEBUG
	DesktopCPPDebugger::UseCosoleBoard();
#endif // DEBUG_MODE

	srand((unsigned)time(NULL));

	int returnedValue = RenderEngine::Window::getInstance().Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "RenderEngine");
	if (returnedValue < 0) {
		printf("Initialize Window::getInstance() failed! Returned (%d).\n", returnedValue);
		exit(returnedValue);
	}
	RenderEngine::LoadBmp(TEXTURE_PATH_4_SHADOWPLANE, RenderEngine::Device::getInstance().pixelColorData4ShadowPlane, RenderEngine::Device::getInstance().textureWidth, RenderEngine::Device::getInstance().textureHeight);
	RenderEngine::LoadBmp(TEXTURE_PATH, RenderEngine::Device::getInstance().pixelColorData, RenderEngine::Device::getInstance().textureWidth, RenderEngine::Device::getInstance().textureHeight);

	
	unsigned int* wndFramebuffer = (unsigned int*)(RenderEngine::Window::getInstance().GetFrameBuffer());
	RenderEngine::Device::getInstance().Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, wndFramebuffer, &RenderEngine::Transform::getInstance());
	RenderEngine::Device::getInstance().SetState(4);
	
	RenderEngine::Transform::getInstance().Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
	
	RenderEngine::ModelInfo objModel;
	RenderEngine::ReadOBJFileIntoOBJModel(MODEL_PATH, &objModel);

	RenderEngine::Device::getInstance().RenderUpdate(objModel);

	RenderEngine::Device::getInstance().Close();
	RenderEngine::Window::getInstance().Close();

	return 0;
}

