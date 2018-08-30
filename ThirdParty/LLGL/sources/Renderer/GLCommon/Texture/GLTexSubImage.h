/*
 * GLTexSubImage.h
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef LLGL_GL_TEX_SUB_IMAGE_H
#define LLGL_GL_TEX_SUB_IMAGE_H


#include <LLGL/ImageFlags.h>
#include <LLGL/TextureFlags.h>


namespace LLGL
{


#ifdef LLGL_OPENGL

void GLTexSubImage1D(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImage2D(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImage3D(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImageCube(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImage1DArray(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImage2DArray(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImageCubeArray(const TextureRegion& region, const SrcImageDescriptor& imageDesc);

#else

void GLTexSubImage2D(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImage3D(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImageCube(const TextureRegion& region, const SrcImageDescriptor& imageDesc);
void GLTexSubImage2DArray(const TextureRegion& region, const SrcImageDescriptor& imageDesc);

#endif


} // /namespace LLGL


#endif



// ================================================================================
