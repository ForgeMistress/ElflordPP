////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Object.cpp
//
//  The base object.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Object.h"

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FireClassIDType::FireClassIDType(const char* name)
	: Name(name)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Mirror);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Metadata Meta(Variant key, Variant value)
{
	return Metadata{
		std::move(key),
		std::move(value)
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FIRE_MIRROR_DEFINE(Firestorm::Mirror::Object)
// {
// }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Object::~Object()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Firestorm);
