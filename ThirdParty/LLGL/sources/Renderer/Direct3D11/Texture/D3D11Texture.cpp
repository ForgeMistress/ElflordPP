/*
 * D3D11Texture.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "D3D11Texture.h"
#include "../../DXCommon/DXCore.h"
#include "../D3D11Types.h"


namespace LLGL
{


D3D11Texture::D3D11Texture(const TextureType type) :
    Texture { type }
{
}

Extent3D D3D11Texture::QueryMipExtent(std::uint32_t mipLevel) const
{
    Extent3D size;

    if (native_.resource)
    {
        D3D11_RESOURCE_DIMENSION dimension;
        native_.resource->GetType(&dimension);

        switch (dimension)
        {
            case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            {
                /* Query MIP-level size for 1D texture */
                D3D11_TEXTURE1D_DESC desc;
                native_.tex1D->GetDesc(&desc);

                if (mipLevel < desc.MipLevels)
                {
                    size.width  = std::max(1u, desc.Width >> mipLevel);
                    size.height = desc.ArraySize;
                    size.depth  = 1u;
                }
            }
            break;

            case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            {
                /* Query MIP-level size for 2D texture */
                D3D11_TEXTURE2D_DESC desc;
                native_.tex2D->GetDesc(&desc);

                if (mipLevel < desc.MipLevels)
                {
                    size.width  = std::max(1u, desc.Width  >> mipLevel);
                    size.height = std::max(1u, desc.Height >> mipLevel);
                    size.depth  = desc.ArraySize;
                }
            }
            break;

            case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            {
                /* Query MIP-level size for 3D texture */
                D3D11_TEXTURE3D_DESC desc;
                native_.tex3D->GetDesc(&desc);

                if (mipLevel < desc.MipLevels)
                {
                    size.width  = std::max(1u, desc.Width  >> mipLevel);
                    size.height = std::max(1u, desc.Height >> mipLevel);
                    size.depth  = std::max(1u, desc.Depth  >> mipLevel);
                }
            }
            break;
        }
    }

    return size;
}

TextureDescriptor D3D11Texture::QueryDesc() const
{
    /* Get D3D hardware texture resource */
    const auto& hwTex = GetNative();

    /* Initialize texture descriptor */
    TextureDescriptor texDesc;

    texDesc.type    = GetType();
    texDesc.flags   = 0;

    /* Get resource dimension to query the respective D3D descriptor */
    D3D11_RESOURCE_DIMENSION dimension;
    hwTex.resource->GetType(&dimension);

    switch (dimension)
    {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
        {
            /* Query descriptor from 1D texture */
            D3D11_TEXTURE1D_DESC desc;
            hwTex.tex1D->GetDesc(&desc);

            texDesc.format      = D3D11Types::Unmap(desc.Format);
            texDesc.extent      = { desc.Width, 1u, 1u };
            texDesc.arrayLayers = desc.ArraySize;
        }
        break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        {
            /* Query descriptor from 2D texture */
            D3D11_TEXTURE2D_DESC desc;
            hwTex.tex2D->GetDesc(&desc);

            texDesc.format      = D3D11Types::Unmap(desc.Format);
            texDesc.extent      = { desc.Width, desc.Height, 1u };
            texDesc.arrayLayers = desc.ArraySize;
            texDesc.samples     = desc.SampleDesc.Count;
        }
        break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
        {
            /* Query descriptor from 3D texture */
            D3D11_TEXTURE3D_DESC desc;
            hwTex.tex3D->GetDesc(&desc);

            texDesc.format  = D3D11Types::Unmap(desc.Format);
            texDesc.extent  = { desc.Width, desc.Height, desc.Depth };
        }
        break;
    }

    return texDesc;
}

static ComPtr<ID3D11Texture1D> DXCreateTexture1D(
    ID3D11Device* device, const D3D11_TEXTURE1D_DESC& desc, const D3D11_SUBRESOURCE_DATA* initialData = nullptr)
{
    ComPtr<ID3D11Texture1D> tex1D;

    auto hr = device->CreateTexture1D(&desc, initialData, &tex1D);
    DXThrowIfCreateFailed(hr, "ID3D11Texture1D");

    return tex1D;
}

static ComPtr<ID3D11Texture2D> DXCreateTexture2D(
    ID3D11Device* device, const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* initialData = nullptr)
{
    ComPtr<ID3D11Texture2D> tex2D;

    auto hr = device->CreateTexture2D(&desc, initialData, &tex2D);
    DXThrowIfCreateFailed(hr, "ID3D11Texture2D");

    return tex2D;
}

static ComPtr<ID3D11Texture3D> DXCreateTexture3D(
    ID3D11Device* device, const D3D11_TEXTURE3D_DESC& desc, const D3D11_SUBRESOURCE_DATA* initialData = nullptr)
{
    ComPtr<ID3D11Texture3D> tex3D;

    auto hr = device->CreateTexture3D(&desc, initialData, &tex3D);
    DXThrowIfCreateFailed(hr, "ID3D11Texture3D");

    return tex3D;
}

void D3D11Texture::CreateTexture1D(
    ID3D11Device*                           device,
    const D3D11_TEXTURE1D_DESC&             desc,
    long                                    flags,
    const D3D11_SUBRESOURCE_DATA*           initialData,
    const D3D11_SHADER_RESOURCE_VIEW_DESC*  srvDesc,
    const D3D11_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
    /* Create native D3D texture, store resource parameters and create standard resource views */
    native_.tex1D = DXCreateTexture1D(device, desc, initialData);
    SetResourceParams(desc.Format, Extent3D{ desc.Width, 1u, 1u }, desc.MipLevels, desc.ArraySize);
    CreateDefaultResourceViews(device, srvDesc, uavDesc, flags);
}

void D3D11Texture::CreateTexture2D(
    ID3D11Device*                           device,
    const D3D11_TEXTURE2D_DESC&             desc,
    long                                    flags,
    const D3D11_SUBRESOURCE_DATA*           initialData,
    const D3D11_SHADER_RESOURCE_VIEW_DESC*  srvDesc,
    const D3D11_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
    /* Create native D3D texture, store resource parameters and create standard resource views */
    native_.tex2D = DXCreateTexture2D(device, desc, initialData);
    SetResourceParams(desc.Format, Extent3D{ desc.Width, desc.Height, 1u }, desc.MipLevels, desc.ArraySize);
    CreateDefaultResourceViews(device, srvDesc, uavDesc, flags);
}

void D3D11Texture::CreateTexture3D(
    ID3D11Device*                           device,
    const D3D11_TEXTURE3D_DESC&             desc,
    long                                    flags,
    const D3D11_SUBRESOURCE_DATA*           initialData,
    const D3D11_SHADER_RESOURCE_VIEW_DESC*  srvDesc,
    const D3D11_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
    /* Create native D3D texture, store resource parameters and create standard resource views */
    native_.tex3D = DXCreateTexture3D(device, desc, initialData);
    SetResourceParams(desc.Format, Extent3D{ desc.Width, desc.Height, desc.Depth }, desc.MipLevels, 1);
    CreateDefaultResourceViews(device, srvDesc, uavDesc, flags);
}

void D3D11Texture::UpdateSubresource(
    ID3D11DeviceContext*        context,
    UINT                        mipLevel,
    UINT                        arrayLayer,
    const D3D11_BOX&            region,
    const SrcImageDescriptor&   imageDesc,
    std::size_t                 threadCount)
{
    /* Get destination subresource index */
    auto dstSubresource = D3D11CalcSubresource(mipLevel, arrayLayer, numMipLevels_);
    auto srcPitch       = DataTypeSize(imageDesc.dataType) * ImageFormatSize(imageDesc.format);

    /* Check if source image must be converted */
    auto dstTexFormat = DXGetTextureFormatDesc(format_);

    if (dstTexFormat.format != imageDesc.format || dstTexFormat.dataType != imageDesc.dataType)
    {
        /* Get source data stride */
        auto srcRowPitch        = (region.right  - region.left ) * srcPitch;
        auto srcDepthPitch      = (region.bottom - region.top  ) * srcRowPitch;
        auto requiredImageSize  = (region.back   - region.front) * srcDepthPitch;

        if (imageDesc.dataSize < requiredImageSize)
        {
            throw std::invalid_argument(
                "image data size is too small to update subresource of D3D11 texture (" +
                std::to_string(requiredImageSize) + " is required but only " + std::to_string(imageDesc.dataSize) + " was specified)"
            );
        }

        /* Convert image data (e.g. from RGB to RGBA) */
        auto tempData = ConvertImageBuffer(imageDesc, dstTexFormat.format, dstTexFormat.dataType, threadCount);

        /* Get new source data stride */
        srcPitch        = DataTypeSize(dstTexFormat.dataType) * ImageFormatSize(dstTexFormat.format);
        srcRowPitch     = (region.right  - region.left) * srcPitch;
        srcDepthPitch   = (region.bottom - region.top ) * srcRowPitch;

        /* Update subresource with specified image data */
        context->UpdateSubresource(
            native_.resource.Get(),
            dstSubresource,
            &region,
            tempData.get(),
            srcRowPitch,
            srcDepthPitch
        );
    }
    else
    {
        /* Get source data stride */
        auto srcRowPitch    = (region.right  - region.left) * srcPitch;
        auto srcDepthPitch  = (region.bottom - region.top ) * srcRowPitch;

        /* Update subresource with specified image data */
        context->UpdateSubresource(
            native_.resource.Get(),
            dstSubresource,
            &region,
            imageDesc.data,
            srcRowPitch,
            srcDepthPitch
        );
    }
}

void D3D11Texture::CreateSubresourceCopyWithCPUAccess(
    ID3D11Device*           device,
    ID3D11DeviceContext*    context,
    D3D11NativeTexture&     textureCopy,
    UINT                    cpuAccessFlags,
    UINT                    mipLevel) const
{
    D3D11_RESOURCE_DIMENSION dimension;
    native_.resource->GetType(&dimension);

    switch (dimension)
    {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
        {
            /* Create temporary 1D texture with a similar descriptor */
            D3D11_TEXTURE1D_DESC desc;
            native_.tex1D->GetDesc(&desc);
            {
                desc.Width          = (desc.Width << mipLevel);
                desc.MipLevels      = 1;
                desc.Usage          = D3D11_USAGE_STAGING;
                desc.BindFlags      = 0;
                desc.CPUAccessFlags = cpuAccessFlags;
                desc.MiscFlags      = (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE);
            }
            textureCopy.tex1D = DXCreateTexture1D(device, desc);
        }
        break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        {
            /* Query and modify descriptor for 2D texture */
            D3D11_TEXTURE2D_DESC desc;
            native_.tex2D->GetDesc(&desc);
            {
                desc.Width          = (desc.Width << mipLevel);
                desc.Height         = (desc.Height << mipLevel);
                desc.MipLevels      = 1;
                desc.Usage          = D3D11_USAGE_STAGING;
                desc.BindFlags      = 0;
                desc.CPUAccessFlags = cpuAccessFlags;
                desc.MiscFlags      = (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE);
            }
            textureCopy.tex2D = DXCreateTexture2D(device, desc);
        }
        break;

        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
        {
            /* Query and modify descriptor for 3D texture */
            D3D11_TEXTURE3D_DESC desc;
            native_.tex3D->GetDesc(&desc);
            {
                desc.Width          = (desc.Width << mipLevel);
                desc.Height         = (desc.Height << mipLevel);
                desc.Depth          = (desc.Depth << mipLevel);
                desc.MipLevels      = 1;
                desc.Usage          = D3D11_USAGE_STAGING;
                desc.BindFlags      = 0;
                desc.CPUAccessFlags = cpuAccessFlags;
                desc.MiscFlags      = (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE);
            }
            textureCopy.tex3D = DXCreateTexture3D(device, desc);
        }
        break;
    }

    /* Copy subresource */
    context->CopySubresourceRegion(
        textureCopy.resource.Get(),
        0,
        0, 0, 0, // DstX, DstY, DstZ
        native_.resource.Get(),
        D3D11CalcSubresource(mipLevel, 0, numMipLevels_),
        nullptr
    );
}

void D3D11Texture::CreateSubresourceSRV(
    ID3D11Device*               device,
    ID3D11ShaderResourceView**  srvOutput,
    UINT                        baseMipLevel,
    UINT                        numMipLevels,
    UINT                        baseArrayLayer,
    UINT                        numArrayLayers)
{
    /* Create SRV for subresource */
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    {
        srvDesc.Format = D3D11Types::ToDXGIFormatSRV(GetFormat());

        switch (GetType())
        {
            case TextureType::Texture1D:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURE1D;
                srvDesc.Texture1D.MostDetailedMip           = baseMipLevel;
                srvDesc.Texture1D.MipLevels                 = numMipLevels;
                break;

            case TextureType::Texture2D:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MostDetailedMip           = baseMipLevel;
                srvDesc.Texture2D.MipLevels                 = numMipLevels;
                break;

            case TextureType::Texture3D:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURE3D;
                srvDesc.Texture3D.MostDetailedMip           = baseMipLevel;
                srvDesc.Texture3D.MipLevels                 = numMipLevels;
                break;

            case TextureType::TextureCube:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURECUBE;
                srvDesc.TextureCube.MostDetailedMip         = baseMipLevel;
                srvDesc.TextureCube.MipLevels               = numMipLevels;
                break;

            case TextureType::Texture1DArray:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
                srvDesc.Texture1DArray.MostDetailedMip      = baseMipLevel;
                srvDesc.Texture1DArray.MipLevels            = numMipLevels;
                srvDesc.Texture1DArray.FirstArraySlice      = baseArrayLayer;
                srvDesc.Texture1DArray.ArraySize            = numArrayLayers;
                break;

            case TextureType::Texture2DArray:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                srvDesc.Texture2DArray.MostDetailedMip      = baseMipLevel;
                srvDesc.Texture2DArray.MipLevels            = numMipLevels;
                srvDesc.Texture2DArray.FirstArraySlice      = baseArrayLayer;
                srvDesc.Texture2DArray.ArraySize            = numArrayLayers;
                break;

            case TextureType::TextureCubeArray:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
                srvDesc.TextureCubeArray.MostDetailedMip    = baseMipLevel;
                srvDesc.TextureCubeArray.MipLevels          = numMipLevels;
                srvDesc.TextureCubeArray.First2DArrayFace   = baseArrayLayer;
                srvDesc.TextureCubeArray.NumCubes           = numArrayLayers;
                break;

            case TextureType::Texture2DMS:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURE2DMS;
                break;

            case TextureType::Texture2DMSArray:
                srvDesc.ViewDimension                       = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
                srvDesc.Texture2DMSArray.FirstArraySlice    = baseArrayLayer;
                srvDesc.Texture2DMSArray.ArraySize          = numArrayLayers;
                break;
        }
    }
    auto hr = device->CreateShaderResourceView(native_.resource.Get(), &srvDesc, srvOutput);
    DXThrowIfCreateFailed(hr, "ID3D11ShaderResourceView", "for texture subresource");
}

void D3D11Texture::CreateSubresourceDSV(
    ID3D11Device*               device,
    ID3D11DepthStencilView**    dsvOutput,
    UINT                        baseMipLevel,
    UINT                        baseArrayLayer,
    UINT                        numArrayLayers)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    {
        dsvDesc.Format  = D3D11Types::ToDXGIFormatDSV(GetFormat());
        dsvDesc.Flags   = 0;

        switch (GetType())
        {
            case TextureType::Texture1D:
                dsvDesc.ViewDimension                       = D3D11_DSV_DIMENSION_TEXTURE1D;
                dsvDesc.Texture1D.MipSlice                  = baseMipLevel;
                break;

            case TextureType::Texture1DArray:
                dsvDesc.ViewDimension                       = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
                dsvDesc.Texture1DArray.MipSlice             = baseMipLevel;
                dsvDesc.Texture1DArray.FirstArraySlice      = baseArrayLayer;
                dsvDesc.Texture1DArray.ArraySize            = numArrayLayers;
                break;

            case TextureType::Texture2D:
                dsvDesc.ViewDimension                       = D3D11_DSV_DIMENSION_TEXTURE2D;
                dsvDesc.Texture2D.MipSlice                  = baseMipLevel;
                break;

            case TextureType::Texture2DArray:
                dsvDesc.ViewDimension                       = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                dsvDesc.Texture2DArray.MipSlice             = baseMipLevel;
                dsvDesc.Texture2DArray.FirstArraySlice      = baseArrayLayer;
                dsvDesc.Texture2DArray.ArraySize            = numArrayLayers;
                break;

            case TextureType::Texture2DMS:
                dsvDesc.ViewDimension                       = D3D11_DSV_DIMENSION_TEXTURE2DMS;
                break;

            case TextureType::Texture2DMSArray:
                dsvDesc.ViewDimension                       = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                dsvDesc.Texture2DMSArray.FirstArraySlice    = baseArrayLayer;
                dsvDesc.Texture2DMSArray.ArraySize          = numArrayLayers;
                break;

            default:
                break;
        }
    }
    auto hr = device->CreateDepthStencilView(native_.resource.Get(), &dsvDesc, dsvOutput);
    DXThrowIfCreateFailed(hr, "ID3D11DepthStencilView",  "for texture subresource");
}


/*
 * ====== Private: ======
 */

void D3D11Texture::CreateDefaultResourceViews(
    ID3D11Device*                           device,
    const D3D11_SHADER_RESOURCE_VIEW_DESC*  srvDesc,
    const D3D11_UNORDERED_ACCESS_VIEW_DESC* uavDesc,
    long                                    flags)
{
    if ((flags & TextureFlags::SampleUsage) != 0)
        CreateDefaultSRV(device, srvDesc);
    if ((flags & TextureFlags::StorageUsage) != 0)
        CreateDefaultUAV(device, uavDesc);
}

void D3D11Texture::CreateDefaultSRV(ID3D11Device* device, const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
    if (!srvDesc && IsDepthStencilFormat(D3D11Types::Unmap(format_)))
    {
        /* Create internal SRV for entire texture resource */
        CreateSubresourceSRV(device, srv_.ReleaseAndGetAddressOf(), 0, numMipLevels_, 0, numArrayLayers_);
    }
    else
    {
        /* Create internal SRV for entire texture resource */
        auto hr = device->CreateShaderResourceView(native_.resource.Get(), srvDesc, srv_.ReleaseAndGetAddressOf());
        DXThrowIfCreateFailed(hr, "ID3D11ShaderResourceView", "for texture");
    }
}

void D3D11Texture::CreateDefaultUAV(ID3D11Device* device, const D3D11_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
    auto hr = device->CreateUnorderedAccessView(native_.resource.Get(), uavDesc, uav_.ReleaseAndGetAddressOf());
    DXThrowIfCreateFailed(hr, "ID3D11UnorderedAccessView", "for texture");
}

void D3D11Texture::SetResourceParams(DXGI_FORMAT format, const Extent3D& extent, UINT mipLevels, UINT arraySize)
{
    format_         = format;
    numMipLevels_   = (mipLevels == 0 ? NumMipLevels(extent.width, extent.height, extent.height) : mipLevels);
    numArrayLayers_ = arraySize;
}


} // /namespace LLGL



// ================================================================================
