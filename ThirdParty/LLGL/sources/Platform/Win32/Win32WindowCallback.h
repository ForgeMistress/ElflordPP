/*
 * Win32WindowCallback.h
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef LLGL_WIN32_WINDOW_CALLBACK_H
#define LLGL_WIN32_WINDOW_CALLBACK_H


#include <Windows.h>


namespace LLGL
{


LRESULT CALLBACK Win32WindowCallback(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);


} // /namespace LLGL


#endif



// ================================================================================
