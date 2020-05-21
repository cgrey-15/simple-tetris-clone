#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector2.h>
#include <array>

namespace supp {
	/*
	* Supporian coordinate system is a pixel-based user-defined coordinate with
	* the positive field being bottom-right from the origin
	*/
	template<Magnum::Int W, Magnum::Int H>
	constexpr Magnum::Vector2 supporianCoordsUnconvert(Magnum::UnsignedInt x, Magnum::UnsignedInt y) {
		Magnum::Float u = static_cast<Magnum::Float>(x) / static_cast<Magnum::Float>(W);
		Magnum::Float v = static_cast<Magnum::Float>(H - y) / static_cast<Magnum::Float>(H);
		return Magnum::Vector2{u, v};
	}

	/*
	* A simple homogeneous container for organizing closely related member data
	*/
	template <typename T, auto N>
	class BasicTaggedTuple {
		static_assert(std::is_enum<decltype(N)>::value, "Not an enum");
		static_assert(std::is_integral<T>::value || std::is_pointer_v<T>, "Only integral types are permitted");
		static constexpr size_t size = static_cast<size_t>(N);

		using enum_type_t = decltype(N);

		std::array<T, size> data_;

	public:
		BasicTaggedTuple() {
			for (int i = 0; i < data_.size(); ++i)
				data_[i] = {};
		}

#if 1
		template<enum_type_t I>
		void toggle() {
			std::get<static_cast<size_t>(I)>(data_) = !std::get<static_cast<size_t>(I)>(data_);
		}
#else
		template<int i = j>
		void toggle(const int j) {
			std::get<i>(data_) = !std::get<i>(data_);
		}
#endif

		template<enum_type_t I>
		typename decltype(data_)::value_type is() {
			return std::get<static_cast<size_t>(I)>(data_);
		}
	};

	template<auto N>
	using StatesTuple = BasicTaggedTuple<bool, N>;

}

#endif