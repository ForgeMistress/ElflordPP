///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkPipeline
//
//  Implementation of the RenderSystem class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VkPipeline.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vk_PipelineLayout::Vk_PipelineLayout(RenderSystem& renderSystem)
: _renderSystem(renderSystem)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vk_PipelineLayout::~Vk_PipelineLayout()
{
	//_renderSystem.Release(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IPipeline::CreateInfo::AddViewport(float posX, float posY,
                                        float extentX, float extentY,
                                        float minDepth, float maxDepth)
{
	Viewports.push_back({ posX,posY, extentX, extentY, minDepth, maxDepth });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IPipeline::CreateInfo IPipeline::MakeCreateInfo(const class RenderSystem& renderSystem,
	                                            IPipelineLayout*          layout,
	                                            IRenderPass*              renderPass)
{
	CreateInfo info;
	const auto& swapchainExtent = renderSystem.GetSwapchainExtent();
	info.AddViewport(
		0,
		0,
		narrow_cast<float>(swapchainExtent.width),
		narrow_cast<float>(swapchainExtent.height),
		0.0f,
		1.0f
	);

	info.Scissors.push_back({
		0,
		0,
		swapchainExtent.width,
		swapchainExtent.height
	});

	info.Layout = layout;
	info.RenderPass = renderPass;

	return info;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vk_Pipeline::Vk_Pipeline(class RenderSystem& renderSystem)
: _renderSystem(renderSystem)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vk_Pipeline::~Vk_Pipeline()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);