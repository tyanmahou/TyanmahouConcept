﻿#pragma once
#include"concept_map.hpp"
#include"require.hpp"
#include"axiom.hpp"
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
		using Concepmagicoheck = decltype(&Concept::template require<Args...>);

		struct void_tester
		{
			template<typename T>
			friend void operator,(T &&, void_tester);
		};
	}


	///<summary>
	///require実装クラスをコンセプト(メタ関数)に変換し継承 SubConceptには派生classを与える
	///</summary>
	template<template<class...>class SubConcept, class Constraint, class ...Args>
	struct to_concept : is_detected<
		detail::Concepmagicoheck,
		Constraint,
		detail::as_mapped_if_t<
		SubConcept<detail::remove_mapped_t<Args>...>,
		Args>...
	>
	{
		using constraint_t = Constraint;
		template<class... AxionArgs>
		static void axiom(AxionArgs&&... args)
		{
			detail::axiom_check<SubConcept<Args...>, Args...>(std::forward<AxionArgs>(args)...);
		}
	};

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
	///Type型が存在するか
	///</summary>
	template<class Type>
	auto associated_type()->decltype(val<Type>());

	///<summary>
	///式がType型で評価可能か
	///</summary>
	///<param name= "exp">
	///評価する式
	///</param>
	template<class Type, class Exp>
	auto valid_expr(Exp&& exp)->magico::require<std::is_convertible<Exp, Type>>;


	constexpr detail::void_tester is_void;

	template<class T>
	auto valid_expr(detail::void_tester)->magico::require<std::is_void<T>>;



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
struct name : magico::to_concept<name,struct __##name##_c,Args...>{};\
template<class ...Args>\
constexpr bool name##_v = name<Args...>::value;\
struct __##name##_c


///<summary>
///暗黙のコンセプトマップを認めない場合
///</summary>
#define MAGICO_CONCEPT_NONE_DEFAULT(name)\
template<class ...Args>\
struct name : magico::to_concept<name,struct __##name##_c,Args...>{};\
template<class ...Args>\
constexpr bool name##_v = name<Args...>::value;\
template<class ...Args>\
struct magico::concept_map< name <Args...>>\
{\
	template<class U>\
	void operator =(U&& v)\
	{}\
};\
struct __##name##_c
