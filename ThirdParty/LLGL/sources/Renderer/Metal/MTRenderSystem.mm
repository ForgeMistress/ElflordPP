/*
 * MTRenderSystem.mm
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MTRenderSystem.h"
#include "../CheckedCast.h"
#include "../../Core/Helper.h"
#include "../../Core/Vendor.h"
#include "MTFeatureSet.h"


namespace LLGL
{


/* ----- Common ----- */

MTRenderSystem::MTRenderSystem()
{
    CreateDeviceResources();
    QueryRenderingCaps();
}

MTRenderSystem::~MTRenderSystem()
{
    [device_ release];
}

/* ----- Render Context ----- */

RenderContext* MTRenderSystem::CreateRenderContext(const RenderContextDescriptor& desc, const std::shared_ptr<Surface>& surface)
{
    return TakeOwnership(renderContexts_, MakeUnique<MTRenderContext>(device_, desc, surface));
}

void MTRenderSystem::Release(RenderContext& renderContext)
{
    RemoveFromUniqueSet(renderContexts_, &renderContext);
}

/* ----- Command queues ----- */

CommandQueue* MTRenderSystem::GetCommandQueue()
{
    return commandQueue_.get();
}

/* ----- Command buffers ----- */

CommandBuffer* MTRenderSystem::CreateCommandBuffer(const CommandBufferDescriptor& /*desc*/)
{
    return TakeOwnership(commandBuffers_, MakeUnique<MTCommandBuffer>(commandQueue_->GetNative()));
}

CommandBufferExt* MTRenderSystem::CreateCommandBufferExt(const CommandBufferDescriptor& /*desc*/)
{
    return TakeOwnership(commandBuffers_, MakeUnique<MTCommandBuffer>(commandQueue_->GetNative()));
}

void MTRenderSystem::Release(CommandBuffer& commandBuffer)
{
    RemoveFromUniqueSet(commandBuffers_, &commandBuffer);
}

/* ----- Buffers ------ */

Buffer* MTRenderSystem::CreateBuffer(const BufferDescriptor& desc, const void* initialData)
{
    return TakeOwnership(buffers_, MakeUnique<MTBuffer>(device_, desc, initialData));
}

BufferArray* MTRenderSystem::CreateBufferArray(std::uint32_t numBuffers, Buffer* const * bufferArray)
{
    AssertCreateBufferArray(numBuffers, bufferArray);
    return TakeOwnership(bufferArrays_, MakeUnique<MTBufferArray>((*bufferArray)->GetType(), numBuffers, bufferArray));
}

void MTRenderSystem::Release(Buffer& buffer)
{
    RemoveFromUniqueSet(buffers_, &buffer);
}

void MTRenderSystem::Release(BufferArray& bufferArray)
{
    RemoveFromUniqueSet(bufferArrays_, &bufferArray);
}

void MTRenderSystem::WriteBuffer(Buffer& dstBuffer, std::uint64_t dstOffset, const void* data, std::uint64_t dataSize)
{
    auto& dstBufferMT = LLGL_CAST(MTBuffer&, dstBuffer);
    dstBufferMT.Write(static_cast<NSUInteger>(dstOffset), data, static_cast<NSUInteger>(dataSize));
}

void* MTRenderSystem::MapBuffer(Buffer& buffer, const CPUAccess access)
{
    return nullptr;//todo
}

void MTRenderSystem::UnmapBuffer(Buffer& buffer)
{
    //todo
}

/* ----- Textures ----- */

Texture* MTRenderSystem::CreateTexture(const TextureDescriptor& textureDesc, const SrcImageDescriptor* imageDesc)
{
    auto textureMT = MakeUnique<MTTexture>(device_, textureDesc);
    
    if (imageDesc)
        textureMT->Write(*imageDesc, { 0, 0, 0 }, textureMT->QueryMipExtent(0));
    
    return TakeOwnership(textures_, std::move(textureMT));
}

void MTRenderSystem::Release(Texture& texture)
{
    RemoveFromUniqueSet(textures_, &texture);
}

void MTRenderSystem::WriteTexture(Texture& texture, const TextureRegion& textureRegion, const SrcImageDescriptor& imageDesc)
{
    //todo
}

void MTRenderSystem::ReadTexture(const Texture& texture, std::uint32_t mipLevel, const DstImageDescriptor& imageDesc)
{
    //todo
}

void MTRenderSystem::GenerateMips(Texture& texture)
{
    auto& textureMT = LLGL_CAST(MTTexture&, texture);
    
    id<MTLCommandBuffer> cmdBuffer = [commandQueue_->GetNative() commandBuffer];
    id<MTLBlitCommandEncoder> blitCmdEncoder = [cmdBuffer blitCommandEncoder];
    
    [blitCmdEncoder generateMipmapsForTexture:textureMT.GetNative()];
    [blitCmdEncoder endEncoding];
    [cmdBuffer commit];
}

void MTRenderSystem::GenerateMips(Texture& texture, std::uint32_t baseMipLevel, std::uint32_t numMipLevels, std::uint32_t baseArrayLayer, std::uint32_t numArrayLayers)
{
    //todo
}

/* ----- Sampler States ---- */

Sampler* MTRenderSystem::CreateSampler(const SamplerDescriptor& desc)
{
    return TakeOwnership(samplers_, MakeUnique<MTSampler>(device_, desc));
}

void MTRenderSystem::Release(Sampler& sampler)
{
    RemoveFromUniqueSet(samplers_, &sampler);
}

/* ----- Resource Heaps ----- */

ResourceHeap* MTRenderSystem::CreateResourceHeap(const ResourceHeapDescriptor& desc)
{
    return TakeOwnership(resourceHeaps_, MakeUnique<MTResourceHeap>(desc));
}

void MTRenderSystem::Release(ResourceHeap& resourceHeap)
{
    RemoveFromUniqueSet(resourceHeaps_, &resourceHeap);
}

/* ----- Render Passes ----- */

RenderPass* MTRenderSystem::CreateRenderPass(const RenderPassDescriptor& desc)
{
    AssertCreateRenderPass(desc);
    return nullptr;//return TakeOwnership(renderPasses_, MakeUnique<MTRenderPass>(desc));
}

void MTRenderSystem::Release(RenderPass& renderPass)
{
    //RemoveFromUniqueSet(renderPasses_, &renderPass);
}

/* ----- Render Targets ----- */

RenderTarget* MTRenderSystem::CreateRenderTarget(const RenderTargetDescriptor& desc)
{
    return nullptr;//todo
}

void MTRenderSystem::Release(RenderTarget& renderTarget)
{
    //todo
}

/* ----- Shader ----- */

Shader* MTRenderSystem::CreateShader(const ShaderDescriptor& desc)
{
    AssertCreateShader(desc);
    return TakeOwnership(shaders_, MakeUnique<MTShader>(device_, desc));
}

ShaderProgram* MTRenderSystem::CreateShaderProgram(const ShaderProgramDescriptor& desc)
{
    AssertCreateShaderProgram(desc);
    return TakeOwnership(shaderPrograms_, MakeUnique<MTShaderProgram>(desc));
}

void MTRenderSystem::Release(Shader& shader)
{
    RemoveFromUniqueSet(shaders_, &shader);
}

void MTRenderSystem::Release(ShaderProgram& shaderProgram)
{
    RemoveFromUniqueSet(shaderPrograms_, &shaderProgram);
}

/* ----- Pipeline Layouts ----- */

PipelineLayout* MTRenderSystem::CreatePipelineLayout(const PipelineLayoutDescriptor& desc)
{
    return TakeOwnership(pipelineLayouts_, MakeUnique<MTPipelineLayout>(desc));
}

void MTRenderSystem::Release(PipelineLayout& pipelineLayout)
{
    RemoveFromUniqueSet(pipelineLayouts_, &pipelineLayout);
}

/* ----- Pipeline States ----- */

GraphicsPipeline* MTRenderSystem::CreateGraphicsPipeline(const GraphicsPipelineDescriptor& desc)
{
    return TakeOwnership(graphicsPipelines_, MakeUnique<MTGraphicsPipeline>(device_, desc));
}

ComputePipeline* MTRenderSystem::CreateComputePipeline(const ComputePipelineDescriptor& desc)
{
    return nullptr;//todo
}

void MTRenderSystem::Release(GraphicsPipeline& graphicsPipeline)
{
    RemoveFromUniqueSet(graphicsPipelines_, &graphicsPipeline);
}

void MTRenderSystem::Release(ComputePipeline& computePipeline)
{
    //todo
    //RemoveFromUniqueSet(computePipelines_, &computePipeline);
}

/* ----- Queries ----- */

QueryHeap* MTRenderSystem::CreateQueryHeap(const QueryHeapDescriptor& desc)
{
    return nullptr;//todo
}

void MTRenderSystem::Release(QueryHeap& queryHeap)
{
    //todo
    //RemoveFromUniqueSet(queryHeaps_, &queryHeap);
}

/* ----- Fences ----- */

Fence* MTRenderSystem::CreateFence()
{
    return nullptr;//todo
}

void MTRenderSystem::Release(Fence& fence)
{
    //todo
    //RemoveFromUniqueSet(fences_, &fence);
}


/*
 * ======= Private: =======
 */

void MTRenderSystem::CreateDeviceResources()
{
    /* Create Metal device */
    device_ = MTLCreateSystemDefaultDevice();
    if (device_ == nil)
        throw std::runtime_error("failed to create Metal device");
    
    /* Initialize renderer information */
    RendererInfo info;
    {
        info.rendererName           = "???";
        info.deviceName             = [[device_ name] cStringUsingEncoding:NSUTF8StringEncoding];
        info.vendorName             = "???";
        info.shadingLanguageName    = "Metal";
    }
    SetRendererInfo(info);
    
    /* Create command queue */
    commandQueue_ = MakeUnique<MTCommandQueue>(device_);
}

static const MTLFeatureSet g_potentialFeatureSets[] =
{
    MTLFeatureSet_macOS_ReadWriteTextureTier2,
    MTLFeatureSet_macOS_GPUFamily1_v3,
    MTLFeatureSet_macOS_GPUFamily1_v2,
    MTLFeatureSet_macOS_GPUFamily1_v1,
};

void MTRenderSystem::QueryRenderingCaps()
{
    RenderingCapabilities caps;
    
    for (auto fset : g_potentialFeatureSets)
    {
        if ([device_ supportsFeatureSet:fset])
        {
            LoadFeatureSetCaps(device_, fset, caps);
            break;
        }
    }

    SetRenderingCaps(caps);
}


} // /namespace LLGL



// ================================================================================
