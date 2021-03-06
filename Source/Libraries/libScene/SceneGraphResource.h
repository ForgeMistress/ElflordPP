///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SceneGraphResource
//
//  Loads up the scene graph for a given mesh.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_SCENEGRAPHRESOURCE_H_
#define LIBSCENE_SCENEGRAPHRESOURCE_H_
#pragma once

#include "RenderMgr.h"

#include <libIO/ResourceMgr.h>
#include <libIO/ResourceLoader.h>
#include <libIO/IResourceObject.h>
#include <libIO/ResourceReference.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SceneGraphLoader final : public ResourceLoader
{
public:
	SceneGraphLoader(RenderMgr& renderMgr);
	virtual ~SceneGraphLoader();

	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref) override;

private:
	RenderMgr&                           _renderMgr;
	Json::CharReaderBuilder              _builder;
	Json::CharReader*                    _reader;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SceneGraphResource final : public IResourceObject
{
	FIRE_RESOURCE_TYPE(SceneGraphResource, SceneGraphLoader);
public:
	struct AssetData
	{
		string Version;
		string Generator;
		string Copyright;
	};

	struct Buffer
	{
		// Handle to the MeshResource it holds.
		Resource MeshResource;
	};

	struct BufferView
	{
		size_t Index;
		size_t ByteLength;
		size_t ByteOffset;
	};
public:
	SceneGraphResource(RenderMgr& renderMgr);
	virtual ~SceneGraphResource();

	virtual bool IsReady() const;

private:
	RenderMgr&                _renderMgr;
	AssetData                 _assetData;
	vector<Buffer>            _buffers;
	vector<BufferView>        _bufferViews;
};

CLOSE_NAMESPACE(Firestorm);

#endif