#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <glad/glad.h>

#include "FutureApp.h"


using namespace Diligent;

std::vector<Texture2D*> Texture2D::m_Cache;


Texture2D::Texture2D(int width, int height, unsigned char* data, int channels) {
    //GLuint textureID;

    m_Width = width;
    m_Height = height;

    // Get render device and immediate context from your engine
    auto renderDevice = FutureApp::m_Inst->GetDevice();
    auto immediateContext = FutureApp::m_Inst->GetContext();

    // Determine texture format based on channels
    TEXTURE_FORMAT textureFormat;
    std::vector<unsigned char> convertedData;
    bool needsConversion = false;

    switch (channels) {
    case 1:
        textureFormat = TEX_FORMAT_R8_UNORM;
        break;
    case 2:
        textureFormat = TEX_FORMAT_RG8_UNORM;
        break;
    case 3:
        // RGB8 might not be supported on all platforms (especially Direct3D)
        // Convert to RGBA8 for maximum compatibility
        textureFormat = TEX_FORMAT_RGBA8_UNORM;
        needsConversion = true;

        // Convert RGB to RGBA

        convertedData.resize(width * height * 4);

        for (int i = 0; i < width * height; ++i) {
            convertedData[i * 4 + 0] = data[i * 3 + 0]; // R
            convertedData[i * 4 + 1] = data[i * 3 + 1]; // G
            convertedData[i * 4 + 2] = data[i * 3 + 2]; // B
            convertedData[i * 4 + 3] = 255;             // A (opaque)
        }

        break;
    case 4:
        textureFormat = TEX_FORMAT_RGBA8_UNORM;
        break;
    default:
        // Handle unsupported format
        return;
    }

    // Create texture description
    TextureDesc texDesc;
    texDesc.Name = "Texture2D";
    texDesc.Type = RESOURCE_DIM_TEX_2D;
    texDesc.Width = static_cast<Uint32>(width);
    texDesc.Height = static_cast<Uint32>(height);
    texDesc.Format = textureFormat;
    texDesc.Usage = USAGE_IMMUTABLE;  // Static texture data
    texDesc.BindFlags = BIND_SHADER_RESOURCE;
    texDesc.MipLevels = 1;  // Single mip level

    // Initialize texture data
    TextureSubResData subResData;
    subResData.pData = needsConversion ? convertedData.data() : data;
    subResData.Stride = static_cast<Uint32>(width * (needsConversion ? 4 : channels));

    TextureData initialData;
    initialData.pSubResources = &subResData;
    initialData.NumSubresources = 1;

    // Create the texture
    renderDevice->CreateTexture(texDesc, &initialData, &m_pTexture);

    // Create a shader resource view
    if (m_pTexture) {
        TextureViewDesc viewDesc;
        viewDesc.ViewType = TEXTURE_VIEW_SHADER_RESOURCE;
        m_pTexture->CreateView(viewDesc, &m_TextureSRV);
    }
}

void Texture2D::ThreadTask() {
    // Do something in the background
    //while (true) {
        // Placeholder work
     //   std::this_thread::sleep_for(std::chrono::seconds(1));
        // Add exit condition or logging as needed
   // }
    TextureLoadInfo loadInfo;
    loadInfo.IsSRGB =false; // Changed from true to false to ensure linear RGB colors
    loadInfo.PermultiplyAlpha = true;
    //loadInfo.



    RefCntAutoPtr<ITexture> Tex;
    CreateTextureFromFile(m_Path.c_str(), loadInfo, FutureApp::m_Inst->GetDevice(), &Tex);
    // Get shader resource view from the texture
    m_TextureSRV = Tex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    m_pTexture = Tex;

    // Get the texture description to retrieve dimensions
    if (m_pTexture) {
        const auto& texDesc = m_pTexture->GetDesc();
        m_Width = static_cast<int>(texDesc.Width);
        m_Height = static_cast<int>(texDesc.Height);
        m_Path = m_Path;
        m_Channels = 4; // Assuming RGBA format
    }
    m_Loaded = true;
}

Texture2D::Texture2D(std::string path,bool mt) {

    if (mt) {
        m_Loaded = false;
        m_Path = path;
        m_Thread = std::thread(&Texture2D::ThreadTask, this);
       
    }
    else {
        TextureLoadInfo loadInfo;
        loadInfo.IsSRGB = false; // Changed from true to false to ensure linear RGB colors
        loadInfo.PermultiplyAlpha = false;

        RefCntAutoPtr<ITexture> Tex;
        CreateTextureFromFile(path.c_str(), loadInfo, FutureApp::m_Inst->GetDevice(), &Tex);
        // Get shader resource view from the texture
        m_TextureSRV = Tex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
        m_pTexture = Tex;

        // Get the texture description to retrieve dimensions
        if (m_pTexture) {
            const auto& texDesc = m_pTexture->GetDesc();
            m_Width = static_cast<int>(texDesc.Width);
            m_Height = static_cast<int>(texDesc.Height);
            m_Path = path;
            m_Channels = 4; // Assuming RGBA format
        }
        m_Loaded = true;
    }
    return;
}

// Constructor for existing Diligent texture
Texture2D::Texture2D(ITexture* pTexture, ITextureView* pTextureView)
{
    // Get the texture description to retrieve dimensions
    const auto& texDesc = pTexture->GetDesc();

    m_Width = static_cast<int>(texDesc.Width);
    m_Height = static_cast<int>(texDesc.Height);
    m_Channels = 4; // Assuming RGBA format
    m_Handle = 0;   // Not using OpenGL handles anymore

    // Store references to Diligent objects
    m_pTexture = pTexture;
    m_TextureSRV = pTextureView;

    // Mark this as an internal texture (no file path)
    m_Path = "internal_texture";
    m_Loaded = true;
}

Texture2D::Texture2D(int w, int h)
{
    m_Width = w;
    m_Height = h;

    // Create an empty texture with Diligent
    auto renderDevice = FutureApp::m_Inst->GetDevice();

    TextureDesc texDesc;
    texDesc.Name = "EmptyTexture";
    texDesc.Type = RESOURCE_DIM_TEX_2D;
    texDesc.Width = static_cast<Uint32>(w);
    texDesc.Height = static_cast<Uint32>(h);
    texDesc.Format = TEX_FORMAT_RGBA8_UNORM; // Linear format
    texDesc.Usage = USAGE_DEFAULT;
    texDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;
    texDesc.MipLevels = 1;

    renderDevice->CreateTexture(texDesc, nullptr, &m_pTexture);

    if (m_pTexture) {
        TextureViewDesc viewDesc;
        viewDesc.ViewType = TEXTURE_VIEW_SHADER_RESOURCE;
        m_pTexture->CreateView(viewDesc, &m_TextureSRV);
    }
}

void Texture2D::Grab(int x, int y) {
    // This functionality would need to be reimplemented using Diligent's CopyTexture or similar
//    std::cout << "Texture2D::Grab not implemented for Diligent Engine" << std::endl;
}

void Texture2D::Bind(int unit) {
    // This is handled differently in Diligent through shader resource bindings
    // No direct implementation needed
}

void Texture2D::Unbind(int unit) {
    // This is handled differently in Diligent through shader resource bindings
    // No direct implementation needed
}

void Texture2D::Free() {
    m_TextureSRV.Release();
    m_pTexture.Release();
    m_TextureSRV = nullptr;
    m_pTexture = nullptr;
}