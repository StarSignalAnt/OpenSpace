#include "RenderTargetCube.h"
#include "FutureApp.h"

using namespace Diligent;

RenderTargetCube::RenderTargetCube(unsigned int size) {
    m_Size = size;

    auto* pDevice = FutureApp::m_Inst->GetDevice();

    // Create the cubemap render target texture
    TextureDesc colorDesc;
    colorDesc.Name = "Cube Map Render Target";
    colorDesc.Type = RESOURCE_DIM_TEX_CUBE;
    colorDesc.Width = size;
    colorDesc.Height = size;
    colorDesc.MipLevels = 1;
    colorDesc.Format = TEX_FORMAT_RG32_FLOAT;
    colorDesc.BindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
    colorDesc.Usage = USAGE_DEFAULT;
    colorDesc.CPUAccessFlags = CPU_ACCESS_NONE;
    colorDesc.ArraySize = 6;

    pDevice->CreateTexture(colorDesc, nullptr, &m_CubeTexture);

    // Create a render target view for each face
    for (Uint32 face = 0; face < 6; ++face) {
        TextureViewDesc rtvDesc;
        rtvDesc.ViewType = TEXTURE_VIEW_RENDER_TARGET;
        rtvDesc.TextureDim = RESOURCE_DIM_TEX_2D_ARRAY;
        rtvDesc.MostDetailedMip = 0;
        rtvDesc.FirstArraySlice = face;
        rtvDesc.NumArraySlices = 1;

        RefCntAutoPtr<ITextureView> rtv;
        m_CubeTexture->CreateView(rtvDesc, &rtv);
        m_RTVs[face] = rtv;
    }

    // Create a shader resource view for the cubemap
    TextureViewDesc srvDesc;
    srvDesc.ViewType = TEXTURE_VIEW_SHADER_RESOURCE;
    srvDesc.TextureDim = RESOURCE_DIM_TEX_CUBE;
    srvDesc.MostDetailedMip = 0;
    srvDesc.NumMipLevels = 1;

    m_CubeTexture->CreateView(srvDesc, &m_SRV);

    // Create the depth texture (2D array with 6 slices)
    TextureDesc depthDesc;
    depthDesc.Name = "Cube Map Depth Buffer";
    depthDesc.Type = RESOURCE_DIM_TEX_2D_ARRAY;
    depthDesc.Width = size;
    depthDesc.Height = size;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 6;
    depthDesc.Format = TEX_FORMAT_D32_FLOAT;
    depthDesc.BindFlags = BIND_DEPTH_STENCIL;
    depthDesc.Usage = USAGE_DEFAULT;

    pDevice->CreateTexture(depthDesc, nullptr, &m_DepthTexture);

    // Create a depth-stencil view for each face
    for (Uint32 face = 0; face < 6; ++face) {
        TextureViewDesc dsvDesc;
        dsvDesc.ViewType = TEXTURE_VIEW_DEPTH_STENCIL;
        dsvDesc.TextureDim = RESOURCE_DIM_TEX_2D_ARRAY;
        dsvDesc.MostDetailedMip = 0;
        dsvDesc.FirstArraySlice = face;
        dsvDesc.NumArraySlices = 1;

        RefCntAutoPtr<ITextureView> dsv;
        m_DepthTexture->CreateView(dsvDesc, &dsv);
        m_DSVs[face] = dsv;
    }
}

void RenderTargetCube::BindFace(unsigned int faceIndex) {
    if (faceIndex >= 6) return;

    auto* pContext = FutureApp::m_Inst->GetContext();
    pContext->SetRenderTargets(1, &m_RTVs[faceIndex], m_DSVs[faceIndex], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    FutureApp::m_Inst->SetBindRTC(this);
}

void RenderTargetCube::ClearFace(unsigned int faceIndex, glm::vec3 color) {
    if (faceIndex >= 6) return;

    auto* pContext = FutureApp::m_Inst->GetContext();
    const float clearColor[4] = { color.r, color.g, color.b, 1.0f };
    pContext->ClearRenderTarget(m_RTVs[faceIndex], clearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    pContext->ClearDepthStencil(m_DSVs[faceIndex], CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void RenderTargetCube::Unbind() {
    auto* pSwapChain = FutureApp::m_Inst->GetSwapChain();
    auto* pRTV = pSwapChain->GetCurrentBackBufferRTV();
    auto* pDSV = pSwapChain->GetDepthBufferDSV();

    FutureApp::m_Inst->GetContext()->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    FutureApp::m_Inst->ClearRTC();
}