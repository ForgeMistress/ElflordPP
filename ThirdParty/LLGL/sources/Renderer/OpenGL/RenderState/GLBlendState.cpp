/*
 * GLBlendState.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "GLBlendState.h"
#include "../Ext/GLExtensions.h"
#include "../../GLCommon/GLExtensionRegistry.h"
#include "../../GLCommon/GLCore.h"
#include "../../GLCommon/GLTypes.h"
#include "../../../Core/HelperMacros.h"
#include "../Texture/GLRenderTarget.h"
#include "GLStateManager.h"
#include <LLGL/GraphicsPipelineFlags.h>


namespace LLGL
{


static void Convert(GLfloat (&dst)[4], const ColorRGBAf& src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}

GLBlendState::GLBlendState(const BlendDescriptor& desc, std::uint32_t numColorAttachments)
{
    Convert(blendColor_, desc.blendFactor);

    sampleAlphaToCoverage_ = desc.alphaToCoverageEnabled;

    if (desc.logicOp != LogicOp::Disabled)
    {
        logicOpEnabled_ = true;
        logicOp_        = GLTypes::Map(desc.logicOp);
    }

    if (desc.independentBlendEnabled)
    {
        GLDrawBufferState::Convert(drawBuffers_[0], desc.targets[0]);
        numDrawBuffers_ = 1;
    }
    else
    {
        numDrawBuffers_ = numColorAttachments;
        for (std::uint32_t i = 0; i < numColorAttachments; ++i)
            GLDrawBufferState::Convert(drawBuffers_[i], desc.targets[i]);
    }
}

void GLBlendState::Bind(GLStateManager& stateMngr)
{
    /* Set blend factor */
    stateMngr.SetBlendColor(blendColor_);
    stateMngr.Set(GLState::SAMPLE_ALPHA_TO_COVERAGE, sampleAlphaToCoverage_);

    if (logicOpEnabled_)
    {
        /* Enable logic pixel operation */
        stateMngr.Enable(GLState::COLOR_LOGIC_OP);
        stateMngr.SetLogicOp(logicOp_);

        /* Bind only color masks for all draw buffers */
        BindDrawBufferColorMasks(stateMngr);
    }
    else
    {
        /* Disable logic pixel operation */
        stateMngr.Disable(GLState::COLOR_LOGIC_OP);

        /* Bind blend states for all draw buffers */
        BindDrawBufferStates(stateMngr);
    }
}

void GLBlendState::BindColorMaskOnly(GLStateManager& stateMngr)
{
    BindDrawBufferColorMasks(stateMngr);
}

int GLBlendState::CompareSWO(const GLBlendState& rhs) const
{
    const auto& lhs = *this;

    LLGL_COMPARE_MEMBER_SWO     ( blendColor_[0]         );
    LLGL_COMPARE_MEMBER_SWO     ( blendColor_[1]         );
    LLGL_COMPARE_MEMBER_SWO     ( blendColor_[2]         );
    LLGL_COMPARE_MEMBER_SWO     ( blendColor_[3]         );
    LLGL_COMPARE_MEMBER_SWO     ( sampleAlphaToCoverage_ );
    LLGL_COMPARE_BOOL_MEMBER_SWO( logicOpEnabled_        );
    LLGL_COMPARE_MEMBER_SWO     ( logicOp_               );
    LLGL_COMPARE_MEMBER_SWO     ( numDrawBuffers_        );

    for (decltype(numDrawBuffers_) i = 0; i < numDrawBuffers_; ++i)
    {
        auto order = GLDrawBufferState::CompareSWO(lhs.drawBuffers_[i], rhs.drawBuffers_[i]);
        if (order != 0)
            return order;
    }

    return 0;
}


/*
 * ======= Private: =======
 */

void GLBlendState::BindDrawBufferStates(GLStateManager& stateMngr)
{
    if (numDrawBuffers_ == 1)
    {
        /* Bind blend states for all draw buffers */
        BindDrawBufferState(drawBuffers_[0]);
    }
    else if (numDrawBuffers_ > 1)
    {
        #ifdef GL_ARB_draw_buffers_blend
        if (HasExtension(GLExt::ARB_draw_buffers_blend))
        {
            /* Bind blend states for respective draw buffers directly via extension */
            for (GLuint i = 0; i < numDrawBuffers_; ++i)
                BindIndexedDrawBufferState(drawBuffers_[i], i);
        }
        else
        #endif // /GL_ARB_draw_buffers_blend
        {
            /* Bind blend states with emulated draw buffer setting */
            for (GLuint i = 0; i < numDrawBuffers_; ++i)
            {
                glDrawBuffer(GLTypes::ToColorAttachment(i));
                BindDrawBufferState(drawBuffers_[i]);
            }

            /* Restore draw buffer settings for current render target */
            if (auto boundRenderTarget = stateMngr.GetBoundRenderTarget())
                boundRenderTarget->SetDrawBuffers();
        }
    }
}

void GLBlendState::BindDrawBufferColorMasks(GLStateManager& stateMngr)
{
    if (numDrawBuffers_ == 1)
    {
        /* Bind color mask for all draw buffers */
        BindDrawBufferColorMask(drawBuffers_[0]);
    }
    else if (numDrawBuffers_ > 1)
    {
        #ifdef GL_EXT_draw_buffers2
        if (HasExtension(GLExt::EXT_draw_buffers2))
        {
            /* Bind color mask for respective draw buffers directly via extension */
            for (GLuint i = 0; i < numDrawBuffers_; ++i)
                BindIndexedDrawBufferColorMask(drawBuffers_[i], i);
        }
        else
        #endif // /GL_EXT_draw_buffers2
        {
            /* Bind color masks with emulated draw buffer setting */
            for (GLuint i = 0; i < numDrawBuffers_; ++i)
            {
                glDrawBuffer(GLTypes::ToColorAttachment(i));
                BindDrawBufferColorMask(drawBuffers_[i]);
            }

            /* Restore draw buffer settings for current render target */
            if (auto boundRenderTarget = stateMngr.GetBoundRenderTarget())
                boundRenderTarget->SetDrawBuffers();
        }
    }
}

void GLBlendState::BindDrawBufferState(const GLDrawBufferState& state)
{
    glColorMask(state.colorMask[0], state.colorMask[1], state.colorMask[2], state.colorMask[3]);
    if (state.blendEnabled)
    {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(state.srcColor, state.dstColor, state.srcAlpha, state.dstAlpha);
        glBlendEquationSeparate(state.funcColor, state.funcAlpha);
    }
    else
        glDisable(GL_BLEND);
}

void GLBlendState::BindIndexedDrawBufferState(const GLDrawBufferState& state, GLuint index)
{
    #ifdef GL_ARB_draw_buffers_blend

    glColorMaski(index, state.colorMask[0], state.colorMask[1], state.colorMask[2], state.colorMask[3]);
    if (state.blendEnabled)
    {
        glEnablei(GL_BLEND, index);
        glBlendFuncSeparatei(index, state.srcColor, state.dstColor, state.srcAlpha, state.dstAlpha);
        glBlendEquationSeparatei(index, state.funcColor, state.funcAlpha);
    }
    else
        glDisablei(GL_BLEND, index);

    #endif // /GL_ARB_draw_buffers_blend
}

void GLBlendState::BindDrawBufferColorMask(const GLDrawBufferState& state)
{
    glColorMask(state.colorMask[0], state.colorMask[1], state.colorMask[2], state.colorMask[3]);
}

void GLBlendState::BindIndexedDrawBufferColorMask(const GLDrawBufferState& state, GLuint index)
{
    #ifdef EXT_draw_buffers2

    glColorMaski(index, state.colorMask[0], state.colorMask[1], state.colorMask[2], state.colorMask[3]);

    #endif // /EXT_draw_buffers2
}


/*
 * GLDrawBufferState struct
 */

void GLBlendState::GLDrawBufferState::Convert(GLDrawBufferState& dst, const BlendTargetDescriptor& src)
{
    dst.blendEnabled    = GLBoolean(src.blendEnabled);
    dst.srcColor        = GLTypes::Map(src.srcColor);
    dst.dstColor        = GLTypes::Map(src.dstColor);
    dst.funcColor       = GLTypes::Map(src.colorArithmetic);
    dst.srcAlpha        = GLTypes::Map(src.srcAlpha);
    dst.dstAlpha        = GLTypes::Map(src.dstAlpha);
    dst.funcAlpha       = GLTypes::Map(src.alphaArithmetic);
    dst.colorMask[0]    = GLBoolean(src.colorMask.r);
    dst.colorMask[1]    = GLBoolean(src.colorMask.g);
    dst.colorMask[2]    = GLBoolean(src.colorMask.b);
    dst.colorMask[3]    = GLBoolean(src.colorMask.a);
}

int GLBlendState::GLDrawBufferState::CompareSWO(const GLDrawBufferState& lhs, const GLDrawBufferState& rhs)
{
    LLGL_COMPARE_BOOL_MEMBER_SWO( blendEnabled );
    LLGL_COMPARE_MEMBER_SWO     ( srcColor     );
    LLGL_COMPARE_MEMBER_SWO     ( dstColor     );
    LLGL_COMPARE_MEMBER_SWO     ( funcColor    );
    LLGL_COMPARE_MEMBER_SWO     ( srcAlpha     );
    LLGL_COMPARE_MEMBER_SWO     ( dstAlpha     );
    LLGL_COMPARE_MEMBER_SWO     ( funcAlpha    );
    LLGL_COMPARE_MEMBER_SWO     ( colorMask[0] );
    LLGL_COMPARE_MEMBER_SWO     ( colorMask[1] );
    LLGL_COMPARE_MEMBER_SWO     ( colorMask[2] );
    LLGL_COMPARE_MEMBER_SWO     ( colorMask[3] );
    return 0;
}


} // /namespace LLGL



// ================================================================================
