/*
 * D3D11RenderContext.h
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef LLGL_D3D11_RENDER_CONTEXT_H
#define LLGL_D3D11_RENDER_CONTEXT_H


#include <LLGL/RenderContext.h>
#include "../DXCommon/ComPtr.h"
#include <d3d11.h>
#include <dxgi.h>


namespace LLGL
{


// Container structure for all D3D11 back buffer objects.
struct D3D11BackBuffer
{
    ComPtr<ID3D11Texture2D>         colorBuffer;
    ComPtr<ID3D11RenderTargetView>  rtv;
    ComPtr<ID3D11Texture2D>         depthStencil;
    ComPtr<ID3D11DepthStencilView>  dsv;
};


class D3D11RenderContext final : public RenderContext
{

    public:

        /* ----- Common ----- */

        D3D11RenderContext(
            IDXGIFactory* factory,
            const ComPtr<ID3D11Device>& device,
            const ComPtr<ID3D11DeviceContext>& context,
            const RenderContextDescriptor& desc,
            const std::shared_ptr<Surface>& surface
        );

        void Present() override;

        Format QueryColorFormat() const override;
        Format QueryDepthStencilFormat() const override;

        const RenderPass* GetRenderPass() const override;

        /* ----- Extended internal functions ----- */

        inline const D3D11BackBuffer& GetBackBuffer() const
        {
            return backBuffer_;
        }

    private:

        bool OnSetVideoMode(const VideoModeDescriptor& videoModeDesc) override;
        bool OnSetVsync(const VsyncDescriptor& vsyncDesc) override;

        void CreateSwapChain(IDXGIFactory* factory);
        void CreateBackBuffer(const VideoModeDescriptor& videoModeDesc);
        void ResizeBackBuffer(const VideoModeDescriptor& videoModeDesc);

        ComPtr<ID3D11Device>        device_;
        ComPtr<ID3D11DeviceContext> context_;

        ComPtr<IDXGISwapChain>      swapChain_;
        UINT                        swapChainInterval_  = 0;
        UINT                        swapChainSamples_   = 1;

        D3D11BackBuffer             backBuffer_;

        DXGI_FORMAT                 colorFormat_        = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT                 depthStencilFormat_ = DXGI_FORMAT_UNKNOWN;

};


} // /namespace LLGL


#endif



// ================================================================================
