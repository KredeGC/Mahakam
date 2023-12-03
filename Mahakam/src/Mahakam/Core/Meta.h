#pragma once

namespace Mahakam::detail
{
	template<size_t I, typename... T>
	struct get_nth_from_variadric_type;

	template<size_t I, typename Head, typename... Tail>
	struct get_nth_from_variadric_type<I, Head, Tail...>
		: get_nth_from_variadric_type<I - 1, Tail...> {};

	template<typename Head, typename... Tail>
	struct get_nth_from_variadric_type<0, Head, Tail...>
	{
		using type = Head;
	};

	template<size_t I, typename... T>
	using get_nth_from_variadric = typename get_nth_from_variadric_type<I, T...>::type;
}