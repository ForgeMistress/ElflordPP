/*
 * VKModuleInterface.h
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "../ModuleInterface.h"
#include "VKRenderSystem.h"


extern "C"
{

LLGL_EXPORT int LLGL_RenderSystem_BuildID()
{
    return LLGL_BUILD_ID;
}

LLGL_EXPORT int LLGL_RenderSystem_RendererID()
{
    return LLGL::RendererID::Vulkan;
}

LLGL_EXPORT const char* LLGL_RenderSystem_Name()
{
    return "Vulkan";
}

LLGL_EXPORT void* LLGL_RenderSystem_Alloc(const void* renderSystemDesc)
{
    auto desc = reinterpret_cast<const LLGL::RenderSystemDescriptor*>(renderSystemDesc);
    return new LLGL::VKRenderSystem(*desc);
}

}



// ================================================================================
