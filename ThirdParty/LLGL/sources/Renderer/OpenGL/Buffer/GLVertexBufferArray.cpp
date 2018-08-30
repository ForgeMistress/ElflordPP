/*
 * GLVertexBufferArray.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "GLVertexBufferArray.h"
#include "GLVertexBuffer.h"
#include "../RenderState/GLStateManager.h"
#include "../../CheckedCast.h"


namespace LLGL
{


GLVertexBufferArray::GLVertexBufferArray() :
    GLBufferArray { BufferType::Vertex }
{
}

void GLVertexBufferArray::BuildVertexArray(std::uint32_t numBuffers, Buffer* const * bufferArray)
{
    /* Bind VAO */
    GLStateManager::active->BindVertexArray(GetVaoID());
    {
        for (std::uint32_t i = 0; numBuffers > 0; --numBuffers)
        {
            auto vertexBufferGL = LLGL_CAST(GLVertexBuffer*, (*bufferArray));
            {
                const auto& vertexFormat = vertexBufferGL->GetVertexFormat();

                /* Bind VBO */
                GLStateManager::active->BindBuffer(GLBufferTarget::ARRAY_BUFFER, vertexBufferGL->GetID());

                /* Build each vertex attribute */
                for (std::uint32_t j = 0, n = static_cast<std::uint32_t>(vertexFormat.attributes.size()); j < n; ++j, ++i)
                    vao_.BuildVertexAttribute(vertexFormat.attributes[j], vertexFormat.stride, i);
            }
            ++bufferArray;
        }
    }
    GLStateManager::active->BindVertexArray(0);
}


} // /namespace LLGL



// ================================================================================
