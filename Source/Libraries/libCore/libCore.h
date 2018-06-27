

#ifndef LIBCORE_H_
#define LIBCORE_H_
#pragma once

#ifdef LIB_CORE
#error "libCore.h already included"
#endif

#define LIB_CORE

#define OPEN_NAMESPACE(ns) namespace ns { extern int ns_##ns
#define CLOSE_NAMESPACE(ns) } // namespace ns

#include <vector>
#include <memory>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <assert.h>
#include <functional>
#include <stdint.h>
#include <type_traits>

OPEN_NAMESPACE(Elf);

template <typename T>
bool is_uninitialized(std::weak_ptr<T> const& weak)
{
	using wt = std::weak_ptr<T>;
	return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
}

using std::cout;
using std::endl;

using std::function;
using std::pair;

template<
	class Ty,
	class Alloc = std::allocator<Ty>
> using Vector = std::vector<Ty, Alloc>;

using String = std::string;

template <
	class Ty,
	class Alloc = std::allocator<Ty>
> using List = std::list<Ty, Alloc>;

template <
	class Kty,
	class Ty,
	class Hasher = std::hash<Kty>,
	class Keyeq = std::equal_to<Kty>,
	class Alloc = std::allocator<std::pair<const Kty, Ty>>
> using UnorderedMap = std::unordered_map<Kty, Ty, Hasher, Keyeq, Alloc>;

template<class Kty,
	class Hasher = std::hash<Kty>,
	class Keyeq = std::equal_to<Kty>,
	class Alloc = std::allocator<Kty>
> using UnorderedSet = std::unordered_set<Kty, Hasher, Keyeq, Alloc>;

template <class T> using SharedPtr = std::shared_ptr<T>;
template <class T> using WeakPtr   = std::weak_ptr<T>;

static const String str_Default;

// Aliases for type_trait structs in type_traits (C++11)
OPEN_NAMESPACE(Traits);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Primary Type Categories
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using IsVoid = std::is_void<T>;
template <class T> using IsNullPointer = std::is_null_pointer<T>;
template <class T> using IsIntegral = std::is_integral<T>;
template <class T> using IsFloatingPoint = std::is_floating_point<T>;
template <class T> using IsArray = std::is_array<T>;
template <class T> using IsEnum = std::is_enum<T>;
template <class T> using IsUnion = std::is_union<T>;
template <class T> using IsPointer = std::is_pointer<T>;
template <class T> using IsClass = std::is_class<T>;
template <class T> using IsLValueReference = std::is_lvalue_reference<T>;
template <class T> using IsRValueReference = std::is_rvalue_reference<T>;
template <class T> using IsMemberObjectPointer = std::is_member_object_pointer<T>;
template <class T> using IsMemberFunctionPointer = std::is_member_function_pointer<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Composite Type Categories
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using IsFundamental = std::is_fundamental<T>;
template <class T> using IsArithmetic = std::is_arithmetic<T>;
template <class T> using IsScalar = std::is_scalar<T>;
template <class T> using IsObject = std::is_object<T>;
template <class T> using IsCompound = std::is_compound<T>;
template <class T> using IsReference = std::is_reference<T>;
template <class T> using IsMemberPointer = std::is_member_pointer<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using IsConst = std::is_const<T>;
template <class T> using IsVolatile = std::is_volatile<T>;
template <class T> using IsTrivial = std::is_trivial<T>;
template <class T> using IsTriviallyCopyable = std::is_trivially_copyable<T>;
template <class T> using IsStandardLayout = std::is_standard_layout<T>;
template <class T> using IsPOD = std::is_pod<T>; // C++20
												 // template <class T> using IsLiteralType                  = std::is_literal_type<T>; // Deprecated by C++17
												 // template <class T> using HasUniqueObjectRepresentations = std::has_unique_object_representations<T>; // C++17
template <class T> using IsEmpty = std::is_empty<T>;
template <class T> using IsPolymorphic = std::is_polymorphic<T>;
template <class T> using IsAbstract = std::is_abstract<T>;
template <class T> using IsFinal = std::is_final<T>;
// template <class T> using IsAggregate = std::is_aggregate<T>;
template <class T> using IsSigned = std::is_signed<T>;
template <class T> using IsUnsigned = std::is_unsigned<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Const-Volatility Specifiers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using RemoveCV = std::remove_cv<T>;
template <class T> using RemoveConst = std::remove_const<T>;
template <class T> using RemoveVolatile = std::remove_volatile<T>;
template <class T> using AddCV = std::add_cv<T>;
template <class T> using AddConst = std::add_const<T>;
template <class T> using AddVolatile = std::add_volatile<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// References
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using RemoveReference = std::remove_reference<T>;
template <class T> using AddLValueReference = std::add_lvalue_reference<T>;
template <class T> using AddRValueReference = std::add_rvalue_reference<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pointers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using RemovePointer = std::remove_pointer<T>;
template <class T> using AddPointer = std::add_pointer<T>;

CLOSE_NAMESPACE(Traits);

CLOSE_NAMESPACE(Elf);

#endif