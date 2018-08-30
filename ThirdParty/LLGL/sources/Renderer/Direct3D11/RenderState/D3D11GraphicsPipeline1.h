/*
 * D3D11GraphicsPipeline1.h
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef LLGL_D3D11_GRAPHICS_PIPELINE1_H
#define LLGL_D3D11_GRAPHICS_PIPELINE1_H

#if LLGL_D3D11_ENABLE_FEATURELEVEL >= 1


#include "D3D11GraphicsPipelineBase.h"
#include <d3d11_1.h>


namespace LLGL
{


// GraphicsPipeline implementation for Direct3D 11.1
class D3D11GraphicsPipeline1 final : public D3D11GraphicsPipelineBase
{

    public:

        D3D11GraphicsPipeline1(
            ID3D11Device1* device,
            const GraphicsPipelineDescriptor& desc
        );

        void Bind(D3D11StateManager& stateMngr) override;

    private:

        void CreateDepthStencilState(ID3D11Device1* device, const DepthDescriptor& depthDesc, const StencilDescriptor& stencilDesc);
        void CreateRasterizerState(ID3D11Device1* device, const RasterizerDescriptor& desc);
        void CreateBlendState(ID3D11Device1* device, const BlendDescriptor& desc);

        ComPtr<ID3D11DepthStencilState> depthStencilState_;
        ComPtr<ID3D11RasterizerState>   rasterizerState_;
        ComPtr<ID3D11BlendState1>       blendState_;

};


} // /namespace LLGL


#endif // /LLGL_D3D11_ENABLE_FEATURELEVEL

#endif



// ================================================================================
