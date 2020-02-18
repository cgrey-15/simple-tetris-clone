#pragma
#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector2.h>

namespace supp {
	/*
	* Supporian coordinate system is a pixel-based user-defined coordinate with
	* the positive field being bottom-right from the origin
	*/
	template<Magnum::Int W, Magnum::Int H>
	Magnum::Vector2 supporianCoordsUnconvert(Magnum::UnsignedInt x, Magnum::UnsignedInt y) {
		Magnum::Float u = static_cast<Magnum::Float>(x) / static_cast<Magnum::Float>(W);
		Magnum::Float v = static_cast<Magnum::Float>(H - y) / static_cast<Magnum::Float>(H);
		return Magnum::Vector2{u, v};
	}


}

#endif