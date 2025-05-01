#include "GP3D.h"
#include "Texture2D.h"
#include "MapHelper.hpp"
#include "Mesh3DBuffer.h"
#include "GraphicsUtilities.h"
#include "FutureApp.h"
#include "NodeLight.h"
#include "NodeCamera.h"
#include <glm/gtc/type_ptr.hpp>
#include "RenderTargetCube.h"

struct R3DConstants
{

    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float4 lightPos;
    float4 lightDiff;
    float4 lightInfo;
    float4 camPos;

};

Diligent::float4x4 GLMToDiligentMatrix3D(const glm::mat4& mat)
{
    // Convert GLM matrix to raw float pointer
    const float* matPtr = glm::value_ptr(mat);

    // Create a Diligent matrix from the data (column-major)
    Diligent::float4x4 diligentMat = Diligent::float4x4::MakeMatrix(matPtr);

    // Transpose it to match HLSL row-major expectation (optional depending on shader layout)
    diligentMat.Transpose();

    return diligentMat;
}

GP3D::GP3D() {

    m_VSPath = "engine/deShader/render3D.vsh";
    m_PSPath = "engine/deShader/render3D.psh";
    CreatePL();

}

void GP3D::CreatePL() {


    auto ps = FutureApp::m_Inst->GetSwapChain();
    auto dev = FutureApp::m_Inst->GetDevice();

    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Render3D PSO";

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
        ShaderCI.Desc.Name = "Render3D VS";
        ShaderCI.FilePath = m_VSPath.c_str();
        dev->CreateShader(ShaderCI, &pVS);
        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        BufferDesc CBDesc;
        CBDesc.Name = "Render3D Constants";
        CBDesc.Size = sizeof(R3DConstants);
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
        ShaderCI.Desc.Name = "Render3D-PS";
        ShaderCI.FilePath = m_PSPath.c_str();
        dev->CreateShader(ShaderCI, &pPS);
    }


    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        // Attribute 0 - vertex position
        LayoutElement{0, 0, 3, VT_FLOAT32, False}, // Pos
        LayoutElement{1,0,3, VT_FLOAT32, False}, // UV
        // Attribute 1 - vertex color
        LayoutElement{2, 0, 4, VT_FLOAT32, False}, // Color
       LayoutElement{3, 0, 3, VT_FLOAT32, False},
        LayoutElement{4, 0, 3, VT_FLOAT32, False},
         LayoutElement{5, 0, 3, VT_FLOAT32, False}
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
                           {SHADER_TYPE_PIXEL, "g_ShadowMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},

    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
    PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

    // clang-format off
    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_WRAP, TEXTURE_ADDRESS_WRAP, TEXTURE_ADDRESS_WRAP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc},
              {SHADER_TYPE_PIXEL, "g_TextureNorm", SamLinearClampDesc},
                  {SHADER_TYPE_PIXEL, "g_ShadowMap", SamLinearClampDesc},

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

void GP3D::Bind(glm::mat4 world, glm::mat4 view, glm::mat4 proj,NodeCamera* cam,NodeLight* light,bool first_light, Mesh3DBuffer* buffer, Texture2D* texture,Texture2D* normal,RenderTargetCube*shadow) {
    // Ensure that the constant buffer is up-to-date for the current frame
    {
        MapHelper<R3DConstants> CBConstants(FutureApp::m_Inst->GetContext(), m_Constants, MAP_WRITE, MAP_FLAG_DISCARD);
        CBConstants[0].world = GLMToDiligentMatrix3D(world);
        CBConstants[0].view = GLMToDiligentMatrix3D(view);
        CBConstants[0].proj = GLMToDiligentMatrix3D(proj);
        CBConstants[0].lightPos = float4(light->GetPosition().x, light->GetPosition().y,light->GetPosition().z, 1);
   //     Diligent::float4 lightPosViewSpace = GLMToDiligentMatrix3D(view) * Diligent::float4(light->GetPosition().x, light->GetPosition().y, light->GetPosition().z, 1);
     //   CBConstants[0].lightPos = lightPosViewSpace;
        CBConstants[0].lightDiff = float4(1, 1, 1, 1);
        CBConstants[0].lightInfo = float4(light->GetRange(), 0, 0, 0);
        CBConstants[0].camPos = float4(cam->GetPosition().x, cam->GetPosition().y, cam->GetPosition().z, 0);
    }

    const Uint64 offset = 0;
    IBuffer* pBuffs[] = { buffer->GetVertexBuffer() };

    // Set texture
    m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(texture->GetView());
    m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_TextureNorm")->Set(normal->GetView());
    m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ShadowMap")->Set(shadow->GetSRV());
// 
    // Ensure vertex and index buffers are correctly set
    FutureApp::m_Inst->GetContext()->SetVertexBuffers(0, 1, pBuffs, &offset, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
    FutureApp::m_Inst->GetContext()->SetIndexBuffer(buffer->GetIndexBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Set pipeline state
    FutureApp::m_Inst->GetContext()->SetPipelineState(m_pPSO);

    // Commit shader resources
    FutureApp::m_Inst->GetContext()->CommitShaderResources(m_SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


}


void GP3D::Render(Mesh3DBuffer* buffer) {


    DrawIndexedAttribs DrawAttrs;     // This is an indexed draw call
    DrawAttrs.IndexType = VT_UINT32; // Index type
    DrawAttrs.NumIndices = buffer->IndexCount();
    // Verify the state of vertex and index buffers
    DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
    FutureApp::m_Inst->GetContext()->DrawIndexed(DrawAttrs);


}
