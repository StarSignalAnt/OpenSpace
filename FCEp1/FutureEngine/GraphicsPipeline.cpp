#include "GraphicsPipeline.h"
#include "FutureApp.h"
#include <fstream>
#include <stdexcept>
#include <string>
// Note: Caller is responsible for deleting the returned memory using delete[]
char* LoadShaderSourceFromFile(const std::string& path)
{
    // Open the file in binary mode and position at the end to get size
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader file: " + path);
    }

    // Get file size
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate memory for the file content (including null terminator)
    char* buffer = new char[fileSize + 1];

    // Read file content
    if (!file.read(buffer, fileSize))
    {
        delete[] buffer;  // Clean up on failure
        throw std::runtime_error("Failed to read shader file: " + path);
    }

    // Add null terminator
    buffer[fileSize] = '\0';

    return buffer;
}


GraphicsPipeline::GraphicsPipeline(std::string vsh,std::string psh) {


    m_VSPath = vsh;
	m_PSPath = psh;
    return;

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = FutureApp::m_Inst->GetSwapChain()->GetDesc().ColorBufferFormat;
    // Use the depth buffer format from the swap chain
    PSOCreateInfo.GraphicsPipeline.DSVFormat = FutureApp::m_Inst->GetSwapChain()->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // No back face culling for this tutorial
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
    // Disable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    // Create a vertex shader
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Triangle vertex shader";
        ShaderCI.Source = LoadShaderSourceFromFile(vsh);
        FutureApp::m_Inst->GetDevice()->CreateShader(ShaderCI, &pVS);

    }

    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Triangle pixel shader";
        ShaderCI.Source = LoadShaderSourceFromFile(psh);
        FutureApp::m_Inst->GetDevice()->CreateShader(ShaderCI, &pPS);
    }

    // Finally, create the pipeline state
    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    FutureApp::m_Inst->GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);

}




void GraphicsPipeline::SetWorld(float4x4 world) {

    m_World = world;


}