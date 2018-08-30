/*
 * OpenGL.h
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef LLGL_OPENGL_H
#define LLGL_OPENGL_H


#include <LLGL/Platform/Platform.h>

#define LLGL_OPENGL

#if defined(LLGL_OS_WIN32)
#   include "../../Platform/Win32/Win32LeanAndMean.h"
#   include <Windows.h>
#   include <GL/GL.h>
#   include <GL/glext.h>
#   include <GL/wglext.h>
#elif defined(LLGL_OS_LINUX)
#   include <GL/gl.h>
#   include <GL/glext.h>
#   include <GL/glx.h>
#elif defined(LLGL_OS_MACOS)
#   include <OpenGL/gl3.h>
#   include <OpenGL/glext.h>
#   include "Platform/MacOS/MacOSGLExt.h"
#else
#   error unsupported platform for OpenGL
#endif


#endif



// ================================================================================
