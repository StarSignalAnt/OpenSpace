#include "FutureApp.h"
#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "GameInput.h"
#include "RenderTarget2D.h"
#include "UIHelp.h"
#include "GameUI.h"
#include "SoundLib.h"


#ifndef ENGINE_DLL
#    define ENGINE_DLL 1
#endif

#ifndef D3D11_SUPPORTED
#    define D3D11_SUPPORTED 0
#endif

#ifndef D3D12_SUPPORTED
#    define D3D12_SUPPORTED 0
#endif

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 0
#endif

#ifndef VULKAN_SUPPORTED
#    define VULKAN_SUPPORTED 0
#endif

#ifndef METAL_SUPPORTED
#    define METAL_SUPPORTED 0
#endif

#if PLATFORM_WIN32
#    define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#if PLATFORM_LINUX
#    define GLFW_EXPOSE_NATIVE_X11 1
#endif

#if PLATFORM_MACOS
#    define GLFW_EXPOSE_NATIVE_COCOA 1
#endif

#if D3D11_SUPPORTED
#    include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#endif
#if D3D12_SUPPORTED
#    include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#endif
#if GL_SUPPORTED
#    include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#endif
#if VULKAN_SUPPORTED
#    include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#endif
#if METAL_SUPPORTED
#    include "Graphics/GraphicsEngineMetal/interface/EngineFactoryMtl.h"
#endif
#if WEBGPU_SUPPORTED
#    include "Graphics/GraphicsEngineWebGPU/interface/EngineFactoryWebGPU.h"
#endif

#include "GLFW/glfw3native.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 512) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            // For repeatable keys (like letter keys), handle as normal
            GameInput::Keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            GameInput::Keys[key] = false;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Adjust the OpenGL viewport to match the new framebuffer size
    //glViewport(0, 0, width, height);
    auto* pSelf = static_cast<FutureApp*>(glfwGetWindowUserPointer(window));
    if (pSelf->GetSwapChain() != nullptr)
        pSelf->GetSwapChain()->Resize(static_cast<Uint32>(width), static_cast<Uint32>(height));

	FutureApp::m_Inst->SetSize(width, height);
    GameUI::m_Inst->SetUISize(width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {

		GameInput::Buttons[button] = true;
        //std::cout << "Mouse button " << button << " pressed\n";
    }
    else if (action == GLFW_RELEASE) {
		GameInput::Buttons[button] = false;
        //std::cout << "Mouse button " << button << " released\n";
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    //std::cout << "Mouse scrolled: " << yoffset << "\n";
    GameInput::MouseDelta.z = yoffset;
}



FutureApp::FutureApp()
{
    m_Inst = this;
}


void FutureApp::SetApp(int width, int height, std::string app)
{
	m_Width = width;
	m_Height = height;
	m_App = app;

    std::cout << "Running " << m_App << " at " << m_Width << "x" << m_Height << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return ;
    }

   

    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwSwapInterval(0);

    // Set OpenGL version (latest available)

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
   // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use Core Profile

    // Create a window
    window = glfwCreateWindow(m_Width, m_Height, m_App.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return ;
    }

    m_Window = window;

    //glfwMakeContextCurrent(window);

    // Load OpenGL function pointers with GLAD
   // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //    std::cerr << "Failed to initialize GLAD\n";
   //     return;
    //}
   // glfwSwapInterval(0); // Disable V-Sync

    // Set viewportsrand(static_cast<unsigned int>(time(0)));  // Seed the random number generator with the current time
    srand(static_cast<unsigned int>(time(0)));  // Seed the random number generator with the current time

   // glViewport(0, 0, m_Width, m_Height);
     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
     glfwSetMouseButtonCallback(window, mouse_button_callback);
     glfwSetScrollCallback(window, scroll_callback);
     glfwSetKeyCallback(window, key_callback);
     SLib = new SoundLIB;
     SLib->initialize();

     glfwSetWindowUserPointer(m_Window, this);
     InitEngine(RENDER_DEVICE_TYPE_D3D12);


    //InitGL();


}

void FutureApp::SetSize(int width, int height) {

   //  glViewport(0, 0, width, height);
    m_Width = width;
    m_Height = height;

}

void FutureApp::InitGL() {

    // Enable blending (for transparent textures, etc.)
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ZERO);  // Common blending mode for transparency

    // Enable depth testing (useful if your 2D game has depth layers, otherwise, you may skip it)
    glEnable(GL_DEPTH_TEST);  // Enable depth testing (if you have 3D elements or Z-ordering)

    // Enable face culling (optional, good for performance if you're only rendering one side of geometry)
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);  // Cull back faces, rendering front faces only

    // Enable alpha testing (optional, for discarding transparent fragments in certain cases)
    glDisable(GL_ALPHA_TEST);
  
    // Set clear color (background color of the window)
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);  // Set background to a dark gray

 

}

int FutureApp::Run()
{

    UIHelp::InitHelp();


    int nextfps = clock() + 1000;

    int ptime = clock();

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Process input
    //    processInput(window);
        glfwPollEvents();
        // Render
        SLib->update();

    //    glClearColor(0, 0, 0, 1);
   //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        frames++;
        int ct = clock();
        if (ct > nextfps) {

            fps = frames;
            frames = 0;
			printf("FPS: %d\n", fps);
            nextfps = ct + 1000;
        }


        if (m_States.size() > 0)
        {
            auto top = m_States.top();

            int ptime2 = clock();

            int ntime = ptime2 - ptime;;
            ptime = ptime2;

            float delta = ((float)ntime) / 1000.0f;

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);


    
	
			GameInput::MouseDelta = -glm::vec3(GameInput::MousePosition.x-xpos,GameInput::MousePosition.y-ypos,-GameInput::MouseDelta.z);
            GameInput::MousePosition = glm::vec2(xpos, ypos);



   

            if (top != nullptr) {
                top->UpdateState(delta);
             //   top->RenderState();
            }



            GameInput::MouseDelta.z = 0;

        }

        // This code should replace the rendering section in the Run method of FutureApp.cpp

           // Get current render target and depth buffer
        auto* pContext = GetContext();
        auto* pSwapchain = GetSwapChain();
        auto dsv = pSwapchain->GetDepthBufferDSV();

        // Set render targets
        ITextureView* pRTV = pSwapchain->GetCurrentBackBufferRTV();
        pContext->SetRenderTargets(1, &pRTV, dsv, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // Clear with linear RGB values (not sRGB)
        const float ClearColor[4] = { 0.5f, 0.0f, 0.0f, 1.0f }; // Pure red in linear space
        pContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        pContext->ClearDepthStencil(dsv, CLEAR_DEPTH_FLAG | CLEAR_STENCIL_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

        // Render the current state
        auto top = m_States.top();
        top->RenderState();

        // Present the frame
        pSwapchain->Present(0);

        // Swap buffers and poll events
        
      //  glfwSwapBuffers(window);
 

    }

    // Clean up and exit
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}

void FutureApp::PushState(FutureState* state)
{
	m_States.push(state);
    state->InitState();
}


void FutureApp::PopState()
{
    auto top = m_States.top();
	m_States.pop();
    top->EndState();
    GameInput::Buttons[0] = false;
    GameInput::Buttons[1] = false;


}

int FutureApp::GetWidth() {

    if (m_BoundRT) {
		return m_BoundRT->GetWidth();
	}
    else {
        return m_Width;
    }
}

int FutureApp::GetHeight() {
    if (m_BoundRT) {
        return m_BoundRT->GetHeight();
    }
    else {
        return m_Height;
    }
}

FutureApp* FutureApp::m_Inst = nullptr;

void FutureApp::SetBind(RenderTarget2D* rt) {
	m_BoundRT = rt;
    if (rt != nullptr) {
//		glViewport(0, 0, rt->GetWidth(), rt->GetHeight());
    }
    else {
	//	glViewport(0, 0, m_Width, m_Height);
    }
}

bool  FutureApp::InitEngine(RENDER_DEVICE_TYPE DevType) {
	// Initialize the rendering engine here
	// This could involve setting up shaders, buffers, etc.
	// For example:
	// m_Renderer = new Renderer(DevType);
	// m_Renderer->Initialize();
#if PLATFORM_WIN32
    Win32NativeWindow Window{ glfwGetWin32Window(m_Window) };
#endif
#if PLATFORM_LINUX
    LinuxNativeWindow Window;
    Window.WindowId = glfwGetX11Window(m_Window);
    Window.pDisplay = glfwGetX11Display();
    if (DevType == RENDER_DEVICE_TYPE_GL)
        glfwMakeContextCurrent(m_Window);
#endif
#if PLATFORM_MACOS
    MacOSNativeWindow Window;
    if (DevType == RENDER_DEVICE_TYPE_GL)
        glfwMakeContextCurrent(m_Window);
    else
        Window.pNSView = GetNSWindowView(m_Window);
#endif

   

    SwapChainDesc SCDesc;
    SCDesc.ColorBufferFormat = TEX_FORMAT_RGBA8_UNORM; // Linear format (not sRGB)
    SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT;   // Linear depth format
    SCDesc.Width = m_Width;
    SCDesc.Height = m_Height;
    SCDesc.BufferCount = 2;          // Double buffering
    SCDesc.PreTransform = SURFACE_TRANSFORM_IDENTITY;
    SCDesc.Usage = SWAP_CHAIN_USAGE_RENDER_TARGET;

    switch (DevType)
    {
#if D3D11_SUPPORTED
    case RENDER_DEVICE_TYPE_D3D11:
    {
#    if ENGINE_DLL
        // Load the dll and import GetEngineFactoryD3D11() function
        auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
#    endif
        auto* pFactoryD3D11 = GetEngineFactoryD3D11();

        EngineD3D11CreateInfo EngineCI;
        pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &m_pDevice, &m_pImmediateContext);
        pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
    }
    break;
#endif // D3D11_SUPPORTED


#if D3D12_SUPPORTED
    case RENDER_DEVICE_TYPE_D3D12:
    {
#    if ENGINE_DLL
        // Load the dll and import GetEngineFactoryD3D12() function
        auto* GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
#    endif
        auto* pFactoryD3D12 = GetEngineFactoryD3D12();

        EngineD3D12CreateInfo EngineCI;
        pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, &m_pImmediateContext);
        pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
    }
    break;
#endif // D3D12_SUPPORTED


#if GL_SUPPORTED
    case RENDER_DEVICE_TYPE_GL:
    {
#    if EXPLICITLY_LOAD_ENGINE_GL_DLL
        // Load the dll and import GetEngineFactoryOpenGL() function
        auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
#    endif
        auto* pFactoryOpenGL = GetEngineFactoryOpenGL();

        EngineGLCreateInfo EngineCI;
        EngineCI.Window = Window;
        pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_pDevice, &m_pImmediateContext, SCDesc, &m_pSwapChain);
    }
    break;
#endif // GL_SUPPORTED


#if VULKAN_SUPPORTED
    case RENDER_DEVICE_TYPE_VULKAN:
    {
#    if EXPLICITLY_LOAD_ENGINE_VK_DLL
        // Load the dll and import GetEngineFactoryVk() function
        auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
#    endif
        auto* pFactoryVk = GetEngineFactoryVk();

        EngineVkCreateInfo EngineCI;
        pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_pDevice, &m_pImmediateContext);
        pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, Window, &m_pSwapChain);
    }
    break;
#endif // VULKAN_SUPPORTED

#if METAL_SUPPORTED
    case RENDER_DEVICE_TYPE_METAL:
    {
        auto* pFactoryMtl = GetEngineFactoryMtl();

        EngineMtlCreateInfo EngineCI;
        pFactoryMtl->CreateDeviceAndContextsMtl(EngineCI, &m_pDevice, &m_pImmediateContext);
        pFactoryMtl->CreateSwapChainMtl(m_pDevice, m_pImmediateContext, SCDesc, Window, &m_pSwapChain);
    }
    break;
#endif // METAL_SUPPORTED

#if WEBGPU_SUPPORTED
    case RENDER_DEVICE_TYPE_WEBGPU:
    {
#    if ENGINE_DLL
        // Load the dll and import LoadGraphicsEngineWebGPU() function
        auto* GetEngineFactoryWGPU = LoadGraphicsEngineWebGPU();
#    endif
        auto* pFactoryWGPU = GetEngineFactoryWGPU();

        EngineWebGPUCreateInfo EngineCI;
        EngineCI.Features.TimestampQueries = DEVICE_FEATURE_STATE_ENABLED;
        pFactoryWGPU->CreateDeviceAndContextsWebGPU(EngineCI, &m_pDevice, &m_pImmediateContext);
        pFactoryWGPU->CreateSwapChainWebGPU(m_pDevice, m_pImmediateContext, SCDesc, Window, &m_pSwapChain);
    }
    break;
#endif // WEBGPU_SUPPORTED

    default:
        std::cerr << "Unknown/unsupported device type";
        return false;
        break;
    }

    if (m_pDevice == nullptr || m_pImmediateContext == nullptr || m_pSwapChain == nullptr)
        return false;

    return true;

}

void FutureApp::ClearZ() {

    if (m_BoundRT != nullptr) {

        m_BoundRT->ClearZ();

    }
    else {

        auto dsv = GetSwapChain()->GetDepthBufferDSV();

        GetContext()->ClearDepthStencil(dsv, CLEAR_DEPTH_FLAG | CLEAR_STENCIL_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
    }

}