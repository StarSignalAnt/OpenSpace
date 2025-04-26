#include "RenderTarget2D.h"
#include "Texture2D.h"
#include <iostream>
#include "FutureApp.h"

RenderTarget2D::RenderTarget2D(unsigned int width, unsigned int height, bool useMipmaps) {
    m_Width = width;
    m_Height = height;
    useMipmaps = false;
    m_UseMipmaps = useMipmaps;

    // Get the render device from FutureApp
    auto* pDevice = FutureApp::m_Inst->GetDevice();

    // Create color attachment texture
    TextureDesc colorTexDesc;
    colorTexDesc.Name = "RenderTarget Color Texture";
    colorTexDesc.Type = RESOURCE_DIM_TEX_2D;
    colorTexDesc.Width = width;
    colorTexDesc.Height = height;
    colorTexDesc.Format = TEX_FORMAT_RGBA8_UNORM; // Ensure linear format (not sRGB)
    colorTexDesc.BindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
    colorTexDesc.Usage = USAGE_DEFAULT;
    colorTexDesc.MipLevels = useMipmaps ? 0 : 1; // 0 for full mipmap chain
    colorTexDesc.CPUAccessFlags = CPU_ACCESS_NONE;

    pDevice->CreateTexture(colorTexDesc, nullptr, &m_ColorTexture);

    // Create depth-stencil attachment texture
    TextureDesc depthTexDesc;
    depthTexDesc.Name = "RenderTarget Depth-Stencil Texture";
    depthTexDesc.Type = RESOURCE_DIM_TEX_2D;
    depthTexDesc.Width = width;
    depthTexDesc.Height = height;
    depthTexDesc.Format = TEX_FORMAT_D24_UNORM_S8_UINT;
    depthTexDesc.BindFlags = BIND_DEPTH_STENCIL;
    depthTexDesc.Usage = USAGE_DEFAULT;
    depthTexDesc.MipLevels = 1; // No mipmaps for depth texture
    depthTexDesc.CPUAccessFlags = CPU_ACCESS_NONE;

    pDevice->CreateTexture(depthTexDesc, nullptr, &m_DepthTexture);

    // Create render target view
    TextureViewDesc rtvDesc;
    rtvDesc.ViewType = TEXTURE_VIEW_RENDER_TARGET;
    rtvDesc.MostDetailedMip = 0;
    m_ColorTexture->CreateView(rtvDesc, &m_RenderTargetView);

    // Create depth-stencil view
    TextureViewDesc dsvDesc;
    dsvDesc.ViewType = TEXTURE_VIEW_DEPTH_STENCIL;
    m_DepthTexture->CreateView(dsvDesc, &m_DepthStencilView);

    // Create shader resource view
    TextureViewDesc srvDesc;
    srvDesc.ViewType = TEXTURE_VIEW_SHADER_RESOURCE;
    srvDesc.MostDetailedMip = 0;
    srvDesc.NumMipLevels = m_UseMipmaps ? m_ColorTexture->GetDesc().MipLevels : 1;
    m_ColorTexture->CreateView(srvDesc, &m_ShaderResourceView);
}

void RenderTarget2D::Bind() {
    // Set this render target as the current render target
    auto* pContext = FutureApp::m_Inst->GetContext();
    pContext->SetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    FutureApp::m_Inst->SetBind(this);
}

void RenderTarget2D::Unbind() {
    // Generate mipmaps if they are enabled
    if (m_UseMipmaps) {
        auto* pContext = FutureApp::m_Inst->GetContext();
        pContext->GenerateMips(m_ShaderResourceView);
    }

    // Unbind this render target by switching back to the swap chain's render target
    auto* pContext = FutureApp::m_Inst->GetContext();
    auto* pSwapChain = FutureApp::m_Inst->GetSwapChain();
    ITextureView* pRTV = pSwapChain->GetCurrentBackBufferRTV();
    ITextureView* pDSV = pSwapChain->GetDepthBufferDSV();
    pContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    FutureApp::m_Inst->SetBind(nullptr);
}

void RenderTarget2D::Clear(glm::vec3 color) {
    auto* pContext = FutureApp::m_Inst->GetContext();
    // Use linear RGB values directly (not sRGB)
    const float ClearColor[4] = { color.x, color.y, color.z, 1.0f };

    pContext->ClearRenderTarget(m_RenderTargetView, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    pContext->ClearDepthStencil(m_DepthStencilView, CLEAR_DEPTH_FLAG | CLEAR_STENCIL_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void RenderTarget2D::Resize(unsigned int width, unsigned int height) {
    if (width == m_Width && height == m_Height) {
        return; // No resize needed
    }

    // Save old settings
    bool useMipmaps = m_UseMipmaps;

    // Free current resources
    Free();

    // Create a new render target with the new size
    *this = RenderTarget2D(width, height, useMipmaps);
}

void RenderTarget2D::SetTexture(Texture2D* texture) {
    if (texture == nullptr) {
        std::cerr << "Cannot set null texture to RenderTarget2D" << std::endl;
        return;
    }

    // Get the texture and view from the Texture2D object
    auto pTexture = texture->GetTexture();
    auto pTextureView = texture->GetView();

    if (!pTexture || !pTextureView) {
        std::cerr << "Invalid texture or texture view in Texture2D" << std::endl;
        return;
    }

    // Check if the texture has the right binding flags
    const auto& texDesc = pTexture->GetDesc();
    if (!(texDesc.BindFlags & BIND_RENDER_TARGET)) {
        std::cerr << "Texture must have BIND_RENDER_TARGET flag to be used as a render target" << std::endl;
        return;
    }

    // Update our references to the new texture
    m_ColorTexture = pTexture;

    // Create a new render target view for this texture
    TextureViewDesc rtvDesc;
    rtvDesc.ViewType = TEXTURE_VIEW_RENDER_TARGET;
    rtvDesc.MostDetailedMip = 0;
    m_ColorTexture->CreateView(rtvDesc, &m_RenderTargetView);

    // Use the provided shader resource view
    m_ShaderResourceView = pTextureView;

    // Update dimensions
    m_Width = texture->GetWidth();
    m_Height = texture->GetHeight();
}

Texture2D* RenderTarget2D::GetTexture() {
    // Create a new Texture2D that wraps our color texture and its shader resource view
    // The caller is responsible for freeing this texture
    return new Texture2D(m_ColorTexture, m_ShaderResourceView);
}

void RenderTarget2D::BindAsTexture(unsigned int textureUnit) {
    // In Diligent, textures are bound via shader resource binding objects
    // This method provides access to the shader resource view that can be used
    // with a shader resource binding object

    // For compatibility with existing code, we'll keep this method
    // but it doesn't perform direct binding as in OpenGL

    // If needed, the caller should use GetSRV() to obtain the texture view
    // and bind it to their shader resource binding object
    std::cout << "Note: In Diligent, use GetSRV() and bind via ShaderResourceBinding instead" << std::endl;
}

void RenderTarget2D::UnbindAsTexture(unsigned int textureUnit) {
    // No direct equivalent in Diligent, as binding is done via shader resource bindings
    // Kept for API compatibility
}

unsigned int RenderTarget2D::GetWidth() {
    return m_Width;
}

unsigned int RenderTarget2D::GetHeight() {
    return m_Height;
}

bool RenderTarget2D::HasMipmaps() {
    return m_UseMipmaps;
}

void RenderTarget2D::Free() {
    // Release all Diligent resources
    m_RenderTargetView.Release();
    m_DepthStencilView.Release();
    m_ShaderResourceView.Release();
    m_ColorTexture.Release();
    m_DepthTexture.Release();

    // Set to nullptr to avoid dangling pointers
    m_RenderTargetView = nullptr;
    m_DepthStencilView = nullptr;
    m_ShaderResourceView = nullptr;
    m_ColorTexture = nullptr;
    m_DepthTexture = nullptr;
}

void RenderTarget2D::ClearZ() {
	auto* pContext = FutureApp::m_Inst->GetContext();
	pContext->ClearDepthStencil(m_DepthStencilView, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}