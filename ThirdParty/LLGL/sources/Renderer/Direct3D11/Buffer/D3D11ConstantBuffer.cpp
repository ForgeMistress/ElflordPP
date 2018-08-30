/*
 * D3D11ConstantBuffer.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "D3D11ConstantBuffer.h"
#include "../../../Core/Helper.h"
#include "../../../Core/Assertion.h"
#include "../../DXCommon/DXCore.h"
#include <algorithm>


namespace LLGL
{


D3D11ConstantBuffer::D3D11ConstantBuffer(ID3D11Device* device, const BufferDescriptor& desc, const void* initialData) :
    D3D11Buffer { BufferType::Constant }
{
    /* Setup descriptor and create constant buffer */
    CD3D11_BUFFER_DESC bufferDesc
    {
        GetAlignedSize(static_cast<UINT>(desc.size), 16u),
        D3D11_BIND_CONSTANT_BUFFER
    };

    if ((desc.flags & BufferFlags::DynamicUsage) != 0)
    {
        bufferDesc.Usage            = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;
    }

    usage_ = bufferDesc.Usage;

    CreateResource(device, bufferDesc, initialData, desc.flags);
    bufferSize_ = static_cast<UINT>(desc.size);
}

void D3D11ConstantBuffer::UpdateSubresource(ID3D11DeviceContext* context, const void* data, UINT dataSize, UINT offset)
{
    /* Validate parameters */
    LLGL_ASSERT_RANGE(dataSize + offset, bufferSize_);

    if (usage_ == D3D11_USAGE_DYNAMIC)
    {
        /* Update partial subresource by mapping buffer from GPU into CPU memory space */
        D3D11_MAPPED_SUBRESOURCE subresource;
        context->Map(GetNative(), 0, (dataSize < bufferSize_ ? D3D11_MAP_WRITE : D3D11_MAP_WRITE_DISCARD), 0, &subresource);
        {
            ::memcpy(reinterpret_cast<char*>(subresource.pData) + offset, data, dataSize);
        }
        context->Unmap(GetNative(), 0);
    }
    else
    {
        /* Update entire subresource */
        if (dataSize == bufferSize_)
            D3D11Buffer::UpdateSubresource(context, data);
        else
            throw std::out_of_range(LLGL_ASSERT_INFO("cannot update D3D11 buffer partially when it is created with static usage"));
    }
}


} // /namespace LLGL



// ================================================================================
