//
//  RenderMgr
//
//  The rendering system manager. Only one of these should exist per Application.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RenderMgr.h"

#include <libIO/Logger.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderMgr::RenderMgr(ResourceMgr& fileIOMgr, ObjectMaker& objectMaker)
: _fileIOMgr(fileIOMgr)
, _objectMaker(objectMaker)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderMgr::~RenderMgr()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderMgr::Initialize(const String& system, const LLGL::RenderContextDescriptor& renderContextDesc)
{
	System = LLGL::RenderSystem::Load(system);
	Context = System->CreateRenderContext(renderContextDesc);

	// Print renderer information
	const auto& info = System->GetRendererInfo();

	if(info.rendererName.find("OpenGL") != String::npos)
	{
		_rendererName = Renderers::OpenGL;
	}
	else if(info.rendererName.find("Direct3D") != String::npos)
	{
		_rendererName = Renderers::Direct3D;
	}

	FIRE_LOG_DEBUG("Renderer:         %s", info.rendererName);
	FIRE_LOG_DEBUG("Device:           %s", info.deviceName);
	FIRE_LOG_DEBUG("Vendor:           %s", info.vendorName);
	FIRE_LOG_DEBUG("Shading Language: %s", info.shadingLanguageName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderMgr::Shutdown()
{
	System->Release(*Context);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenderMgr::IsUsingRenderer(const String& api)
{
	String renderer(GetRenderer());
	if(renderer.find(api) != String::npos)
	{
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const String& RenderMgr::GetRenderer() const
{
	return _rendererName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const String& RenderMgr::GetDevice() const
{
	return System->GetRendererInfo().deviceName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const String& RenderMgr::GetVendor() const
{
	return System->GetRendererInfo().vendorName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const String& RenderMgr::GetShadingLanguageName() const
{
	return System->GetRendererInfo().shadingLanguageName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);