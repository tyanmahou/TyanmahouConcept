﻿#pragma once
#include"concept_map.hpp"
#include"require.hpp"


//************************************************************************************************
//
//constraint
//
//************************************************************************************************

namespace magico
{

	namespace detail
	{
		template<class Concept, class ... Args>
		using Conceptcheck = decltype(&Concept::template require<Args...>);

		struct void_tester
		{
			template<typename T>
			friend void operator,(T &&, void_tester);
		};

		template<class Constraint, class ...Args>
		struct to_concept_impl : is_detected<
			detail::Conceptcheck,
			Constraint,
			Args...
		>
		{
			using constraint_t = Constraint;
		};
	}

	///<summary>
	///require実装クラスをコンセプト(メタ関数)に変換する
	///</summary>
	template<class Constraint, class ...Args>
	using to_concept = detail::to_concept_impl<
		Constraint,
		detail::as_mapped_if_t<
		detail::to_concept_impl<Constraint, detail::remove_mapped_t<Args>...>,
		Args>...
	>;

	/////<summary>
	/////require実装クラスをコンセプト(メタ関数)に変換し継承 SubConceptには派生classを与える
	/////</summary>
	//template<template<class...>class SubConcept, class Constraint, class ...Args>
	//struct to_concept_ex : detail::to_concept_impl<
	//	Constraint,
	//	detail::as_mapped_if_t<
	//	SubConcept<detail::remove_mapped_t<Args>...>,
	//	Args>...
	//>
	//{};

	/**************************************************************

	require helper

	***************************************************************/

	///<summary>
	///制約の継承
	///</summary>
	template<template<class...>class ...Concepts>
	struct extends
	{
		template<class... Args>
		static auto require(Args&&...)->magico::require<Concepts<Args...>...>;
		template<class... Args>
		static auto require()->magico::require<Concepts<Args...>...>;

	};

	///<summary>
	///Type型が有効か
	///</summary>
	template<class Type>
	auto valid_type()->decltype(val<Type>());

	///<summary>
	///式がType型に変換可能で有効か
	///</summary>
	///<param name= "exp">
	///評価する式
	///</param>
	template<class Type, class Exp>
	auto valid_expr(Exp&& exp)->magico::require<std::is_convertible<Exp, Type>>;

	constexpr detail::void_tester is_void;

	template<class T>
	auto valid_expr(detail::void_tester)->magico::require<std::is_void<T>>;


	///<summary>
	///式の型がConceptを満たし有効か
	///</summary>
	///<param name= "exp">
	///評価する式
	///</param>
	template<template<class...>class Concept, class Exp>
	auto valid_expr(Exp&& exp)->decltype(magico::extends<Concept>::require(std::forward<Exp>(exp)));


}//namespace magico
//************************************************************************************************
//
//macro
//
//************************************************************************************************


///<summary>
///条件をみたさないとアサート
///</summary>
#define MAGICO_CONCEPT_ASSERT( ... ) static_assert(magico::And<__VA_ARGS__>::value,#__VA_ARGS__ )

///<summary>
///条件をみたさないとアサート(bool)
///</summary>
#define MAGICO_CONCEPT_ASSERT_BOOL( ... ) static_assert(static_cast<bool>(__VA_ARGS__),#__VA_ARGS__ )

///<summary>
///コンセプト生成
///</summary>
#define MAGICO_CONCEPT(name)\
template<class ...Args>\
using name = magico::to_concept<struct name##_constraint,Args...>;\
template<class ...Args>\
constexpr bool name##_v = name<Args...>::value;\
struct name##_constraint


///<summary>
///暗黙のコンセプトマップを認めない場合
///</summary>
#define MAGICO_CONCEPT_MAP_NONE_DEFAULT(name)\
template<class ...Args>\
struct magico::concept_map<name<Args...>>\
{\
	template<class T>\
	static void apply(T&& other)\
	{}\
}
