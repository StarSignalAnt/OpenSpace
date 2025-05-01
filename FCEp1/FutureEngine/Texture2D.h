#pragma once
#include <string>
#include <vector>

#include "RefCntAutoPtr.hpp"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"
#include <thread>
#include "FutureApp.h"

using namespace Diligent;

class Texture2D
{
public:
    // Existing constructors
    Texture2D(std::string path,bool mt = false);
    Texture2D(int width, int height, unsigned char* data, int channels);
    Texture2D(int handle, int w, int h)
    {
        m_Handle = handle;
        m_Width = w;
        m_Height = h;
    }
    Texture2D(int w, int h);

    // New constructor for Diligent texture
    Texture2D(ITexture* pTexture, ITextureView* pTextureView);

    void Grab(int x, int y);

    void Bind(int unit);
    void Unbind(int unit);
    int GetID() {
        return m_Handle;
    }
    void Free();
    std::string GetPath() {
        return m_Path;
    }
    int GetWidth() { return m_Width; }
    int GetHeight() { return m_Height; }

    static std::vector<Texture2D*> m_Cache;
    RefCntAutoPtr<ITextureView> GetView() {
        if (!m_Loaded) {
            return FutureApp::m_Inst->m_WhiteTex->GetView();
        }
        return m_TextureSRV;
    }

    // Get the actual Diligent texture
    RefCntAutoPtr<ITexture> GetTexture() {
        if (!m_Loaded)
        {
            return FutureApp::m_Inst->m_WhiteTex->GetTexture();
        }
        return m_pTexture;
    }

private:
    int m_Handle;
    int m_Width, m_Height, m_Channels;
    std::string m_Path;
    RefCntAutoPtr<ITextureView> m_TextureSRV;
    RefCntAutoPtr<ITexture> m_pTexture;
    bool m_Loaded = false;
    std::thread m_Thread;
    void ThreadTask();
};