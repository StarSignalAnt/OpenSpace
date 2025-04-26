#pragma once

#include <glm/glm.hpp>
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "TextureView.h"
#include "Texture.h"

using namespace Diligent;

class Texture2D;

class RenderTarget2D {
public:
    RenderTarget2D(unsigned int width, unsigned int height, bool useMipmaps = true);

    void Bind();
    void Unbind();
    void Clear(glm::vec3 color);
    void ClearZ();
    void Resize(unsigned int width, unsigned int height);

    void SetTexture(Texture2D* texture);
    Texture2D* GetTexture();

    // Helper methods for using the render target as a texture
    void BindAsTexture(unsigned int textureUnit = 0);
    void UnbindAsTexture(unsigned int textureUnit = 0);

    unsigned int GetWidth();
    unsigned int GetHeight();
    bool HasMipmaps();

    void Free();

    // Diligent specific methods
    ITextureView* GetRTV() { return m_RenderTargetView; }
    ITextureView* GetDSV() { return m_DepthStencilView; }
    ITextureView* GetSRV() { return m_ShaderResourceView; }
    ITexture* GetColorTexture() { return m_ColorTexture; }

private:
    RefCntAutoPtr<ITexture> m_ColorTexture;    // Color attachment
    RefCntAutoPtr<ITexture> m_DepthTexture;    // Depth-stencil attachment
    RefCntAutoPtr<ITextureView> m_RenderTargetView;  // Render target view
    RefCntAutoPtr<ITextureView> m_DepthStencilView;  // Depth-stencil view 
    RefCntAutoPtr<ITextureView> m_ShaderResourceView; // SRV for the color texture

    unsigned int m_Width;
    unsigned int m_Height;
    bool m_UseMipmaps;
};