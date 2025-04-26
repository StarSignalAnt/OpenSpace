#pragma once
#include "FutureState.h"
#include <string>
#include <vector>
#include <stack>

#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "RenderStateNotationLoader.h"
#include "BasicMath.hpp"
class GLFWwindow;
class RenderTarget2D;
class SoundLIB;
class Texture2D;

using namespace Diligent;

class FutureApp

{
public:

	FutureApp();
	void SetApp(int width, int height, std::string app);
	int Run();
	void InitGL();
	void PushState(FutureState* state);
	void PopState();
	void SetSize(int width, int height);
	int GetWidth();
	int GetHeight();
	bool InitEngine(RENDER_DEVICE_TYPE DevType);
	void SetBind(RenderTarget2D* rt);
	RenderTarget2D* GetBind() { return m_BoundRT; };
	static FutureApp* m_Inst;
	SoundLIB* SLib;
	IEngineFactory* GetEngineFactory() { return m_pDevice->GetEngineFactory(); }
	IRenderDevice* GetDevice() { return m_pDevice; }
	IDeviceContext* GetContext() { return m_pImmediateContext; }
	ISwapChain* GetSwapChain() { return m_pSwapChain; }
	void ClearZ();
private:

	int m_Width;
	int m_Height;
	std::string m_App;
	std::stack<FutureState*> m_States;
	GLFWwindow* window;
	int UpdateInerval;
	//fps
	int frames = 0;
	int fps = 0;
	RenderTarget2D* m_BoundRT;
	Texture2D* m_Icon;

	//DE
	RefCntAutoPtr<IRenderDevice>  m_pDevice;
	RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
	RefCntAutoPtr<ISwapChain>     m_pSwapChain;
	GLFWwindow* m_Window = nullptr;
};

