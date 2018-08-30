/*
 * D3D11RenderContext.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "D3D11RenderContext.h"
#include "D3D11RenderSystem.h"
#include <LLGL/Platform/NativeHandle.h>
#include <LLGL/Log.h>
#include "../../Core/Helper.h"
#include "../DXCommon/DXTypes.h"


namespace LLGL
{


D3D11RenderContext::D3D11RenderContext(
    IDXGIFactory* factory,
    const ComPtr<ID3D11Device>& device,
    const ComPtr<ID3D11DeviceContext>& context,
    const RenderContextDescriptor& desc,
    const std::shared_ptr<Surface>& surface) :
        RenderContext     { desc.videoMode, desc.vsync       },
        device_           { device                           },
        context_          { context                          },
        swapChainSamples_ { desc.multiSampling.SampleCount() }
{
    /* Setup surface for the render context */
    SetOrCreateSurface(surface, desc.videoMode, nullptr);

    /* Create D3D objects */
    CreateSwapChain(factory);
    CreateBackBuffer(GetVideoMode());

    /* Initialize v-sync */
    OnSetVsync(desc.vsync);
}

void D3D11RenderContext::Present()
{
    swapChain_->Present(swapChainInterval_, 0);
}

Format D3D11RenderContext::QueryColorFormat() const
{
    return DXTypes::Unmap(colorFormat_);
}

Format D3D11RenderContext::QueryDepthStencilFormat() const
{
    return DXTypes::Unmap(depthStencilFormat_);
}

const RenderPass* D3D11RenderContext::GetRenderPass() const
{
    return nullptr; // dummy
}

//TODO: depth-stencil and color format does not change, only resizing is considered!
bool D3D11RenderContext::OnSetVideoMode(const VideoModeDescriptor& videoModeDesc)
{
    const auto& prevVideoMode = GetVideoMode();

    /* Resize back buffer */
    if (prevVideoMode.resolution != videoModeDesc.resolution)
        ResizeBackBuffer(videoModeDesc);

    /* Switch fullscreen mode */
    #if 0
    if (prevVideoMode.fullscreen != videoModeDesc.fullscreen)
    {
        auto hr = swapChain_->SetFullscreenState(videoModeDesc.fullscreen ? TRUE : FALSE, nullptr);
        return SUCCEEDED(hr);
    }
    #else
    /* Switch fullscreen mode */
    if (!SwitchFullscreenMode(videoModeDesc))
        return false;
    #endif

    return true;
}

bool D3D11RenderContext::OnSetVsync(const VsyncDescriptor& vsyncDesc)
{
    swapChainInterval_ = (vsyncDesc.enabled ? std::max(1u, std::min(vsyncDesc.interval, 4u)) : 0u);
    return true;
}


/*
 * ======= Private: =======
 */

static UINT FintSuitableMultisamples(ID3D11Device* device, DXGI_FORMAT format, UINT sampleCount)
{
    for (; sampleCount > 1; --sampleCount)
    {
        UINT numLevels = 0;
        if (device->CheckMultisampleQualityLevels(format, sampleCount, &numLevels) == S_OK)
        {
            if (numLevels > 0)
                return sampleCount;
        }
    }
    return 1;
}

void D3D11RenderContext::CreateSwapChain(IDXGIFactory* factory)
{
    /* Get current settings */
    const auto& videoMode = GetVideoMode();
    const auto& vsync = GetVsync();

    /* Pick and store color format */
    colorFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_B8G8R8A8_UNORM

    /* Find suitable multi-samples for color format */
    auto sampleCount = FintSuitableMultisamples(device_.Get(), colorFormat_, swapChainSamples_);
    if (swapChainSamples_ != sampleCount)
    {
        Log::PostReport(
            Log::ReportType::Information,
            (
                "reduced multi-samples for anti-aliasing from " +
                std::to_string(swapChainSamples_) + " to " + std::to_string(sampleCount)
            )
        );
        swapChainSamples_ = sampleCount;
    }

    /* Create swap chain for window handle */
    NativeHandle wndHandle;
    GetSurface().GetNativeHandle(&wndHandle);

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    InitMemory(swapChainDesc);
    {
        swapChainDesc.BufferDesc.Width                      = videoMode.resolution.width;
        swapChainDesc.BufferDesc.Height                     = videoMode.resolution.height;
        swapChainDesc.BufferDesc.Format                     = colorFormat_;
        swapChainDesc.BufferDesc.RefreshRate.Numerator      = vsync.refreshRate;
        swapChainDesc.BufferDesc.RefreshRate.Denominator    = vsync.interval;
        swapChainDesc.SampleDesc.Count                      = swapChainSamples_;
        swapChainDesc.SampleDesc.Quality                    = 0;
        swapChainDesc.BufferUsage                           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount                           = (videoMode.swapChainSize == 3 ? 2 : 1);
        swapChainDesc.OutputWindow                          = wndHandle.window;
        swapChainDesc.Windowed                              = TRUE;//(videoMode.fullscreen ? FALSE : TRUE);
        swapChainDesc.SwapEffect                            = DXGI_SWAP_EFFECT_DISCARD;
    }
    auto hr = factory->CreateSwapChain(device_.Get(), &swapChainDesc, swapChain_.ReleaseAndGetAddressOf());
    DXThrowIfFailed(hr, "failed to create DXGI swap chain");
}

void D3D11RenderContext::CreateBackBuffer(const VideoModeDescriptor& videoModeDesc)
{
    HRESULT hr = 0;

    /* Pick and store depth-stencil format */
    depthStencilFormat_ = DXPickDepthStencilFormat(videoModeDesc.depthBits, videoModeDesc.stencilBits);

    /* Get back buffer from swap chain */
    hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffer_.colorBuffer.ReleaseAndGetAddressOf()));
    DXThrowIfFailed(hr, "failed to get D3D11 back buffer from swap chain");

    /* Create back buffer RTV */
    hr = device_->CreateRenderTargetView(backBuffer_.colorBuffer.Get(), nullptr, backBuffer_.rtv.ReleaseAndGetAddressOf());
    DXThrowIfFailed(hr, "failed to create D3D11 render-target-view (RTV) for back buffer");

    #if 0//TODO: allow to avoid a depth-buffer
    if (videoModeDesc.depthBits > 0 || videoModeDesc.stencilBits > 0)
    #endif
    {
        /* Create depth stencil texture */
        D3D11_TEXTURE2D_DESC texDesc;
        {
            texDesc.Width               = videoModeDesc.resolution.width;
            texDesc.Height              = videoModeDesc.resolution.height;
            texDesc.MipLevels           = 1;
            texDesc.ArraySize           = 1;
            texDesc.Format              = depthStencilFormat_;
            texDesc.SampleDesc.Count    = swapChainSamples_;
            texDesc.SampleDesc.Quality  = 0;
            texDesc.Usage               = D3D11_USAGE_DEFAULT;
            texDesc.BindFlags           = D3D11_BIND_DEPTH_STENCIL;
            texDesc.CPUAccessFlags      = 0;
            texDesc.MiscFlags           = 0;
        }
        hr = device_->CreateTexture2D(&texDesc, nullptr, backBuffer_.depthStencil.ReleaseAndGetAddressOf());
        DXThrowIfFailed(hr, "failed to create D3D11 depth-texture for swap-chain");

        /* Create DSV */
        hr = device_->CreateDepthStencilView(backBuffer_.depthStencil.Get(), nullptr, backBuffer_.dsv.ReleaseAndGetAddressOf());
        DXThrowIfFailed(hr, "failed to create D3D11 depth-stencil-view (DSV) for swap-chain");
    }
}

void D3D11RenderContext::ResizeBackBuffer(const VideoModeDescriptor& videoModeDesc)
{
    /* Check if the current RTV and DSV is from this render context */
    /*ID3D11RenderTargetView* rtv = nullptr;
    ID3D11DepthStencilView* dsv = nullptr;

    context_->OMGetRenderTargets(1, &rtv, &dsv);

    bool rtvFromThis = (rtv == backBuffer_.rtv.Get() && dsv == backBuffer_.dsv.Get());

    auto r1 = rtv->Release();
    auto r2 = dsv->Release();*/

    /* Unset render targets (if the current RTV and DSV was from this render context) */
    //if (rtvFromThis)
        context_->OMSetRenderTargets(0, nullptr, nullptr);

    /* Release buffers */
    backBuffer_.colorBuffer.Reset();
    backBuffer_.rtv.Reset();
    backBuffer_.depthStencil.Reset();
    backBuffer_.dsv.Reset();

    /* Resize swap-chain buffers, let DXGI find out the client area, and preserve buffer count and format */
    auto hr = swapChain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    DXThrowIfFailed(hr, "failed to resize DXGI swap-chain buffers");

    /* Recreate back buffer and reset default render target */
    CreateBackBuffer(videoModeDesc);

    /* Reset render target (if the previous RTV and DSV was from this render context) */
    //if (rtvFromThis)
    /*{
        ID3D11RenderTargetView* rtvList[] = { backBuffer_.rtv.Get() };
        context_->OMSetRenderTargets(1, rtvList, backBuffer_.dsv.Get());
    }*/
}


} // /namespace LLGL



// ================================================================================
