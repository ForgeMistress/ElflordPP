/*
 * GLBufferArray.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "GLBufferArray.h"
#include "GLBuffer.h"
#include "../../CheckedCast.h"
#include "../../../Core/Helper.h"


namespace LLGL
{


GLBufferArray::GLBufferArray(const BufferType type) :
    BufferArray { type }
{
}

GLBufferArray::GLBufferArray(const BufferType type, std::uint32_t numBuffers, Buffer* const * bufferArray) :
    BufferArray { type }
{
    BuildArray(numBuffers, bufferArray);
}


/*
 * ======= Protected: =======
 */

void GLBufferArray::BuildArray(std::uint32_t numBuffers, Buffer* const * bufferArray)
{
    /* Store the ID of each GLBuffer inside the array */
    idArray_.clear();
    idArray_.reserve(numBuffers);
    while (auto next = NextArrayResource<GLBuffer>(numBuffers, bufferArray))
        idArray_.push_back(next->GetID());
}


} // /namespace LLGL



// ================================================================================
