///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ObjectMaker
//
//  Implements a factory for objects.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ObjectMaker.h"

OPEN_NAMESPACE(Firestorm);

ObjectMaker::ObjectMaker()
{
}

ObjectMaker::~ObjectMaker()
{
	std::scoped_lock lock(_allLock);
	for(auto makerPair : _makers)
	{
		delete makerPair.second;
	}
	_makers.clear();
}

bool ObjectMaker::RegisterMaker(FireClassID key, IMaker* maker)
{
	std::scoped_lock lock(_allLock);
	if(_makers.find(key) == _makers.end())
	{
		_makers[key] = maker;
		return true;
	}
	return false;
}

void* ObjectMaker::Make(FireClassID key)
{
	IMaker* maker = GetMaker(key);
	return maker->Make();
}

bool ObjectMaker::IsMakerRegistered(FireClassID type) const
{
	return _makers.find(type) != _makers.end();
}

IMaker* ObjectMaker::GetMaker(FireClassID type) const
{
	std::scoped_lock<std::mutex> lock(_allLock);
	auto found = _makers.find(type);
	if(found != _makers.end())
		return (*found).second;
	return nullptr;
}

CLOSE_NAMESPACE(Firestorm);