#pragma once

namespace Mahakam::detail
{
	template<unsigned int I, typename... T>
	struct get_nth_from_variadric_type;

	template<unsigned int I, typename Head, typename... Tail>
	struct get_nth_from_variadric_type<I, Head, Tail...>
		: get_nth_from_variadric_type<I - 1, Tail...> {};

	template<typename Head, typename... Tail>
	struct get_nth_from_variadric_type<0, Head, Tail...>
	{
		using type = Head;
	};

	template<unsigned int I, typename... T>
	using get_nth_from_variadric = typename get_nth_from_variadric_type<I, T...>::type;
}