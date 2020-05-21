#ifndef __ROT_GEN_CONSTEXPR_HPP__
#define __ROT_GEN_CONSTEXPR_HPP__
#include<tuple>
#include "ctla/matrix.h"
namespace {
	//using mat3c = ctla::matrix<int8_t, 3, 3>;


	using _magic_num_t = int;
	using _vector2ct = std::pair<_magic_num_t, _magic_num_t>;
	template<typename T>
	using _basic_vector_t = std::pair<T, T>;
	using _mat3 = ctla::matrix<_magic_num_t, 3, 3>;


	template<typename T>
	constexpr auto v(_basic_vector_t<T> v2) {
		return ctla::matrix<T, 3, 1>{ {v2.first, v2.second, 1} };
	}
	template<typename T>
	constexpr auto tup(const ctla::matrix<T, 3, 1> m3) {
		return _basic_vector_t<T>{ m3[0], m3[1] };
	}

	constexpr _mat3 cc90 =
	{ {0, -1, 0,
		1, 0, 0,
		0, 0, 1}
	};
	template<int8_t X>
	constexpr _mat3 dispos =
	{ {1, 0, -X,
	   0, 1, -X,
	   0, 0,  1} };


	template<int8_t X>
	constexpr _mat3 deDispos = { {
			1, 0, X,
			0, 1, X,
			0, 0, 1
} };
	using silly_t = decltype(deDispos<1>);
	constexpr ctla::matrix<int8_t, 3, 1> wuh = { {0, 1, 1} };


	constexpr _mat3 rot0 = deDispos<1> * dispos<1>;
	template<_magic_num_t S>
	constexpr _mat3 rot90 = deDispos<S> * cc90 * dispos<S>;
	template<_magic_num_t S>
	constexpr _mat3 rot180 = deDispos<S> * cc90 * cc90 * dispos<S>;
	template<_magic_num_t S>
	constexpr _mat3 rot270 = deDispos<S> * cc90 * cc90 * cc90 * dispos<S>;


	template<_magic_num_t S>
	constexpr auto eastward(const tetris_clone::Vector2c pair) {
		return tup(rot270<S> * v(pair));
	}
	template<_magic_num_t S>
	constexpr auto southward(const tetris_clone::Vector2c pair) {
		return tup(rot180<S> * v(pair));
	}
	template<_magic_num_t S>
	constexpr auto westward(const tetris_clone::Vector2c pair) {
		return tup(rot90<S> * v(pair));
	}

	template<typename T, typename U, size_t N, _magic_num_t S, size_t ...Idx>
	constexpr const std::array<T, N> aggr_westward_impl(const std::array<U, N>& items, std::index_sequence<Idx...>) {
		return { ::westward<S>(std::get<Idx>(items))... };
	}

	template<typename T, typename U, size_t N, _magic_num_t S=1>
	constexpr const std::array<T, N> aggr_westward(const std::array<U, N>& items) {
		return aggr_westward_impl<T, S>(items, std::make_index_sequence<N>());
	}
	template<typename T, _magic_num_t S, typename U, size_t N, size_t ...Idx>
	constexpr auto all_dir_test_impl(const std::array<U, N>& items, std::index_sequence<Idx...>)->const std::array<std::array<T,N>, 4> {
		return { {{{ std::get<Idx>(items)... }},
		{ {::eastward<S>(std::get<Idx>(items))...} },
		{ {::southward<S>(std::get<Idx>(items))...} },
		{ {::westward<S>(std::get<Idx>(items))...}}} };
	}

	template<typename T, _magic_num_t S=1, typename U, size_t N>
	constexpr auto all_dir_test(const std::array<U, N>& items)->const std::array<std::array<T,N>, 4> {
		return all_dir_test_impl<T, S>(items, std::make_index_sequence<N>());
	}


	//constexpr std::array<tetris_clone::Vector2i, 4> SEA = { {{ 0, 1 }, { 1, 1 }, { 1, 2 }, { 2, 2 }} };

	//constexpr auto snow = all_dir_test<tetris_clone::Vector2c>(SEA);
}


#endif // __ROT_GEN_CONSTEXPR_HPP__
