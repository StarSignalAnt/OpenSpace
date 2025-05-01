#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "TextureView.h"
#include "Texture.h"

using namespace Diligent;

class RenderTargetCube {
public:
    RenderTargetCube(unsigned int size);

    // Bind a specific cube face for rendering
    void BindFace(unsigned int faceIndex);

    void Unbind();

    // Clear a specific cube face
    void ClearFace(unsigned int faceIndex, glm::vec3 color = glm::vec3(0.0f));

    // Get shader resource view for sampling in shaders
    Diligent::ITextureView* GetSRV() const { return m_SRV; }

    // Get the underlying texture (optional)
    Diligent::ITexture* GetTexture() const { return m_CubeTexture; }

    unsigned int GetSize() const { return m_Size; }

private:
    unsigned int m_Size;

    Diligent::RefCntAutoPtr<Diligent::ITexture> m_CubeTexture;
    Diligent::RefCntAutoPtr<Diligent::ITexture> m_DepthTexture;

    std::array<Diligent::RefCntAutoPtr<Diligent::ITextureView>, 6> m_RTVs;
    std::array<Diligent::RefCntAutoPtr<Diligent::ITextureView>, 6> m_DSVs;

    Diligent::RefCntAutoPtr<Diligent::ITextureView> m_SRV;
};