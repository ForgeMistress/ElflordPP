/*
 * D3D12Buffer.h
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef LLGL_D3D12_BUFFER_H
#define LLGL_D3D12_BUFFER_H


#include <LLGL/Buffer.h>
#include "../../DXCommon/ComPtr.h"
#include <d3d12.h>


namespace LLGL
{


class D3D12Buffer : public Buffer
{

    public:

        void UpdateStaticSubresource(
            ID3D12Device*               device,
            ID3D12GraphicsCommandList*  commandList,
            ComPtr<ID3D12Resource>&     uploadBuffer,
            const void*                 data,
            UINT64                      bufferSize,
            UINT64                      offset,
            D3D12_RESOURCE_STATES       stateAfter
        );

        void UpdateDynamicSubresource(const void* data, UINT64 bufferSize, UINT64 offset);

        //! Returns the native ID3D12Resource object.
        inline ID3D12Resource* GetNative() const
        {
            return resource_.Get();
        }

        //! Returns the size (in bytes) of the hardware buffer.
        inline UINT64 GetBufferSize() const
        {
            return bufferSize_;
        }

    protected:

        D3D12Buffer(const BufferType type);

        void CreateResource(ID3D12Device* device, UINT64 bufferSize, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceState);
        void CreateResource(ID3D12Device* device, UINT64 bufferSize);

    private:

        ComPtr<ID3D12Resource>  resource_;
        UINT64                  bufferSize_ = 0;

};


} // /namespace LLGL


#endif



// ================================================================================
