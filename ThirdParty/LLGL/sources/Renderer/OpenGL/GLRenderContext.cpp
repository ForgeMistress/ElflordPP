/*
 * GLRenderContext.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "GLRenderContext.h"


namespace LLGL
{


GLRenderContext::GLRenderContext(RenderContextDescriptor desc, const std::shared_ptr<Surface>& surface, GLRenderContext* sharedRenderContext) :
    RenderContext  { desc.videoMode, desc.vsync                           },
    contextHeight_ { static_cast<GLint>(desc.videoMode.resolution.height) }
{
    #ifdef __linux__

    /* Setup surface for the render context and pass native context handle */
    NativeContextHandle windowContext;
    GetNativeContextHandle(windowContext, desc.videoMode, desc.multiSampling);
    SetOrCreateSurface(surface, desc.videoMode, &windowContext);

    #else

    /* Setup surface for the render context */
    SetOrCreateSurface(surface, desc.videoMode, nullptr);

    #endif

    /* Update video mode of descriptor after surface has been set or created */
    desc.videoMode = GetVideoMode();

    /* Create platform dependent OpenGL context */
    GLContext* sharedGLContext = (sharedRenderContext != nullptr ? sharedRenderContext->context_.get() : nullptr);
    context_ = GLContext::Create(desc, GetSurface(), sharedGLContext);

    /* Setup swap interval (for v-sync) */
    OnSetVsync(desc.vsync);

    /* Get state manager and notify about the current render context */
    stateMngr_ = context_->GetStateManager();
    stateMngr_->NotifyRenderTargetHeight(contextHeight_);

    /* Initialize render states for the first time */
    if (!sharedRenderContext)
        InitRenderStates();
}

void GLRenderContext::Present()
{
    context_->SwapBuffers();
}

Format GLRenderContext::QueryColorFormat() const
{
    /* Return fixed value, not much of control for an OpenGL context */
    return Format::RGBA8UNorm;
}

Format GLRenderContext::QueryDepthStencilFormat() const
{
    /* Return fixed value, not much of control for an OpenGL context */
    return Format::D24UNormS8UInt;
}

const RenderPass* GLRenderContext::GetRenderPass() const
{
    return nullptr; // dummy
}

bool GLRenderContext::GLMakeCurrent(GLRenderContext* renderContext)
{
    if (renderContext)
    {
        /* Make OpenGL context of the specified render contex current and notify the state manager */
        auto result = GLContext::MakeCurrent(renderContext->context_.get());
        GLStateManager::active->NotifyRenderTargetHeight(renderContext->contextHeight_);
        return result;
    }
    else
        return GLContext::MakeCurrent(nullptr);
}


/*
 * ======= Private: =======
 */

bool GLRenderContext::OnSetVideoMode(const VideoModeDescriptor& videoModeDesc)
{
    /* Update context height */
    contextHeight_ = static_cast<GLint>(videoModeDesc.resolution.height);
    stateMngr_->NotifyRenderTargetHeight(contextHeight_);

    /* Notify GL context of a resize */
    context_->Resize(videoModeDesc.resolution);

    /* Switch fullscreen mode */
    if (!SwitchFullscreenMode(videoModeDesc))
        return false;

    return true;
}

bool GLRenderContext::OnSetVsync(const VsyncDescriptor& vsyncDesc)
{
    int swapInterval = (vsyncDesc.enabled ? static_cast<int>(vsyncDesc.interval) : 0);
    return context_->SetSwapInterval(swapInterval);
}

void GLRenderContext::InitRenderStates()
{
    /* Initialize state manager */
    stateMngr_->Reset();

    /* Setup default render states to be uniform between render systems */
    stateMngr_->Enable(GLState::TEXTURE_CUBE_MAP_SEAMLESS); // D3D10+ has this per default
    stateMngr_->SetFrontFace(GL_CW);                        // D3D10+ uses clock-wise vertex winding per default

    /*
    Set pixel storage to byte-alignment (default is word-alignment).
    This is required so that texture formats like RGB (which is not word-aligned) can be used.
    */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1); //???
}


} // /namespace LLGL



// ================================================================================
