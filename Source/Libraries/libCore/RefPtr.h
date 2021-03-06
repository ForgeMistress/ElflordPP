///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RefPtr
//
//  Reference counted smart pointer.
//  Adapted from http://www.acodersjourney.com/implementing-smart-pointer-using-reference-counting/\
//  With some tidbits from https://codereview.stackexchange.com/questions/99087/alternate-weak-pointer-implementation
//
//  Season to taste.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_REFPTR_H_
#define LIBCORE_REFPTR_H_
#pragma once

#include "libCore.h"
#include "Assert.h"

OPEN_NAMESPACE(Firestorm);

template<class T>
using RefPtr = eastl::shared_ptr<T>;

template<class T, class Deleter = std::default_delete<T>>
using UniquePtr = eastl::unique_ptr<T, Deleter>;

template<class T>
using WeakPtr = eastl::weak_ptr<T>;

CLOSE_NAMESPACE(Firestorm);

#endif