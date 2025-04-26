#include "GPMap2D.h"

#include "FutureApp.h"
#include "MapHelper.hpp"
#include "MeshBuffer.h"
#include "GraphicsUtilities.h"
#include "Texture2D.h"

struct M2Constants
{

    float4x4 world;
    float4 screenSize;
    float4 lightPos;
    float4 lightActual;
    float4 lightRange;
    float4 camRot;
    float4 camZoom;
    float4 lightDiffuse;
    float4 mapSize;
    float4 topY;

};

GPMap2D::GPMap2D() {

    m_VSPath = "engine/deShader/mapLight.vsh";
    m_PSPath = "engine/deShader/mapLight.psh";
    CreatePL();

}

void GPMap2D::CreatePL() {

    auto ps = FutureApp::m_Inst->GetSwapChain();
    auto dev = FutureApp::m_Inst->GetDevice();

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "SmartDraw(Map2D) PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = ps->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat = ps->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0].BlendEnable = True;


    // Source factors
    PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0].SrcBlend = BLEND_FACTOR_SRC_ALPHA;
    PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0].SrcBlendAlpha = BLEND_FACTOR_ONE;

    // Destination factors
    PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0].DestBlend = BLEND_FACTOR_INV_SRC_ALPHA;
    PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0].DestBlendAlpha = BLEND_FACTOR_ONE;

    // Blend operations
    //PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0].BlendOp = BLEND_OPERATION_ADD;
   // PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0].BlendOpAlpha = BLEND_OPERATION_ADD;


    // Specify which color channels to write (typically all for alpha blending)
    PSOCreateInfo.GraphicsPipeline.BlendDesc.RenderTargets[0].RenderTargetWriteMask =
        COLOR_MASK_RED | COLOR_MASK_GREEN | COLOR_MASK_BLUE | COLOR_MASK_ALPHA;

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;

    // Pack matrices in row-major order
    ShaderCI.CompileFlags = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;

    // Presentation engine always expects input in gamma space. Normally, pixel shader output is
    // converted from linear to gamma space by the GPU. However, some platforms (e.g. Android in GLES mode,
    // or Emscripten in WebGL mode) do not support gamma-correction. In this case the application
    // has to do the conversion manually.
  //  ShaderMacro Macros[] = { {"CONVERT_PS_OUTPUT_TO_GAMMA", m_ConvertPSOutputToGamma ? "1" : "0"} };
  //  ShaderCI.Macros = { Macros, _countof(Macros) };

    // In this tutorial, we will load shaders from file. To be able to do that,
    // we need to create a shader source stream factory
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;

    FutureApp::m_Inst->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);

    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
    // Create a vertex shader
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "Cube VS";
        ShaderCI.FilePath = m_VSPath.c_str();
        dev->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        BufferDesc CBDesc;
        CBDesc.Name = "GPMap2D Constants";
        CBDesc.Size = sizeof(M2Constants);
        CBDesc.Usage = USAGE_DYNAMIC;
        CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
        CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
        dev->CreateBuffer(CBDesc, nullptr, &m_Constants);
    }

    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint = "main";
        ShaderCI.Desc.Name = "GP3D PS";
        ShaderCI.FilePath = m_PSPath.c_str();
        dev->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        // Attribute 0 - vertex position
        LayoutElement{0, 0, 3, VT_FLOAT32, False}, // Pos
        LayoutElement{1,0,2, VT_FLOAT32, False}, // UV
        // Attribute 1 - vertex color
        LayoutElement{2, 0, 4, VT_FLOAT32, False}, // Color
       LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 4, VT_FLOAT32, False},
         LayoutElement{5, 0, 4, VT_FLOAT32, False}
    };
    // clang-format on
    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    ShaderResourceVariableDesc Vars[] =
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
            {SHADER_TYPE_PIXEL, "g_TextureNorm", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
                {SHADER_TYPE_PIXEL, "g_ShadowMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc},
             {SHADER_TYPE_PIXEL, "g_TextureNorm", SamLinearClampDesc},
                        {SHADER_TYPE_PIXEL, "g_ShadowMap", SamLinearClampDesc}
    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);



    dev->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);


    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables never
    // change and are bound directly through the pipeline state object.
    m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_Constants);

    // Create a shader resource binding object and bind all static resources in it
    m_pPSO->CreateShaderResourceBinding(&m_SRB, true);

}

void GPMap2D::Bind() {


    {
        MapHelper<float4x4> CBConstants(FutureApp::m_Inst->GetContext(), m_Constants, MAP_WRITE, MAP_FLAG_DISCARD);
        *CBConstants = m_World;
    }

    FutureApp::m_Inst->GetContext()->SetPipelineState(m_pPSO);



}

void GPMap2D::Bind(MeshBuffer* buffer, Texture2D* texture) {


    {
        MapHelper<M2Constants> CBConstants(FutureApp::m_Inst->GetContext(), m_Constants, MAP_WRITE, MAP_FLAG_DISCARD);
		CBConstants[0].world = m_World;
        CBConstants[0].camRot = camRot;
		CBConstants[0].camZoom = camZoom;
		CBConstants[0].lightPos = lightPos;
		CBConstants[0].lightActual = lightActual;
		CBConstants[0].lightRange = lightRange;
		CBConstants[0].lightDiffuse = lightDiffuse;
		CBConstants[0].mapSize = mapSize;
		CBConstants[0].topY = topY;
        CBConstants[0].screenSize = screenSize;


    }

    const Uint64 offset = 0;
    IBuffer* pBuffs[] = { buffer->GetVertexBuffer() };


    m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(texture->GetView());
    if (m_AuxTexture != nullptr) {
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureNorm")->Set(m_AuxTexture->GetView());
    }
    if (m_OtherTexture != nullptr) {
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ShadowMap")->Set(m_OtherTexture->GetView());
    }




    FutureApp::m_Inst->GetContext()->SetVertexBuffers(0, 1, pBuffs, &offset, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
    FutureApp::m_Inst->GetContext()->SetIndexBuffer(buffer->GetIndexBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    FutureApp::m_Inst->GetContext()->SetPipelineState(m_pPSO);

    FutureApp::m_Inst->GetContext()->CommitShaderResources(m_SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);






}


void GPMap2D::Render() {

    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 3; // We will render 3 vertices
    FutureApp::m_Inst->GetContext()->Draw(drawAttrs);

}

void GPMap2D::Render(MeshBuffer* buffer) {


    DrawIndexedAttribs DrawAttrs;     // This is an indexed draw call
    DrawAttrs.IndexType = VT_UINT32; // Index type
    DrawAttrs.NumIndices = buffer->IndexCount() * 3;
    // Verify the state of vertex and index buffers
    DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
    FutureApp::m_Inst->GetContext()->DrawIndexed(DrawAttrs);


}
