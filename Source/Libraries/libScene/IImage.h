///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IImage
//
//  IImage
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_IImage_H_
#define LIBSCENE_IImage_H_
#pragma once

#include <libCore/Object.h>

#include <libIO/IResourceObject.h>
#include <EASTL/optional.h>

OPEN_NAMESPACE(Firestorm);

class IImage : public IResourceObject
{
	FIRE_MIRROR_DECLARE(IImage, IResourceObject);
public:
	struct CreateInfo
	{
		optional<vector<char>> Data;

	};
	virtual ~IImage() = default;

private:
};

CLOSE_NAMESPACE(Firestorm);


#ifdef FIRE_GFX_VULKAN
#include "Vulkan/VkImage.h"
#endif

#endif