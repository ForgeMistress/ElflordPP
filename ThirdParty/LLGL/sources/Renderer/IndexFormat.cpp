/*
 * IndexFormat.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <LLGL/IndexFormat.h>
#include <LLGL/RenderSystemFlags.h>


namespace LLGL
{


IndexFormat::IndexFormat(const DataType dataType) :
    formatSize_ { DataTypeSize(dataType) }
{
}


} // /namespace LLGL



// ================================================================================
