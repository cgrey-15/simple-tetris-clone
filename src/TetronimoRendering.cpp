#include "TetronimoRendering.h"

#include "Sprite.h"

#include <Magnum/Math/Color.h>

#include <Magnum/Timeline.h>
#include <Magnum/Math/Matrix.h>

namespace {
	const static Magnum::Vector2i positions[7][4] =
	{
		{{ 0, 1 },
{ 1, 1 },
{ 1, 2 },
{ 2, 2 }},

		{{ 0, 2 },
{ 1, 2 },
{ 1, 1 },
{ 2, 1 }},

		{{ 1, 2 },
{ 0, 1 },
{ 1, 1 },
{ 2, 1 }},

		{{ 0, 1 },
{ 1, 1 },
{ 2, 1 },
{ 2, 2 }},

		{{ 0, 2 },
{ 0, 1 },
{ 1, 1 },
{ 2, 1 }},

		{{ 1, 2 },
{ 2,2 },
{ 3, 2 },
{ 4, 2 }},

		{{ 1, 2 },
{ 2, 2 },
{ 2, 1 },
{ 1, 1 }} };


const Magnum::Vector2i* getPositions(tetris_clone::Tetronimo::Flavor flavor) {
	const Magnum::Vector2i* result{};
	switch (flavor) {
	case tetris_clone::Tetronimo::Flavor::S: result = positions[0];
							 break;
	case tetris_clone::Tetronimo::Flavor::Z: result = positions[1];
							 break;
	case tetris_clone::Tetronimo::Flavor::T: result = positions[2];
							 break;
	case tetris_clone::Tetronimo::Flavor::J: result = positions[3];
							 break;
	case tetris_clone::Tetronimo::Flavor::L: result = positions[4];
							 break;
	case tetris_clone::Tetronimo::Flavor::I: result = positions[5];
							 break;
	case tetris_clone::Tetronimo::Flavor::O: result = positions[6];
							 break;
	}
	return result;

}
constexpr uint16_t maxsizey = std::numeric_limits<short>::max();

}

void tetris_clone::TetronimoRendering::redoTetronimoRender(tetris_clone::Tetronimo& tetronimi, const Magnum::Vector2 * src, Magnum::Int size, bool makeVisible = true) {
	_transform.resetTransformation();
	_transform.translate({_xScale*tetronimi.x(), _yScale*tetronimi.y()});

	auto tetri = getPositions(tetronimi.flavor());
#if 0
	Magnum::Vector2i positions[4];

	switch (tetronimi.flavor()) {
	case Tetronimo::Flavor::S: positions[0] = { 0, 1 };
							 positions[1] = { 1, 1 };
							 positions[2] = { 1, 2 };
							 positions[3] = { 2, 2 };
							 break;
	case Tetronimo::Flavor::Z: positions[0] = { 0, 2 };
							 positions[1] = { 1, 2 };
							 positions[2] = { 1, 1 };
							 positions[3] = { 2, 1 };
							 break;
	case Tetronimo::Flavor::T: positions[0] = { 1, 2 };
							 positions[1] = { 0, 1 };
							 positions[2] = { 1, 1 };
							 positions[3] = { 2, 1 };
							 break;
	case Tetronimo::Flavor::J: positions[0] = { 0, 1 };
							 positions[1] = { 1, 1 };
							 positions[2] = { 2, 1 };
							 positions[3] = { 2, 2 };
							 break;
	case Tetronimo::Flavor::L: positions[0] = { 0, 2 };
							 positions[1] = { 0, 1 };
							 positions[2] = { 1, 1 };
							 positions[3] = { 2, 1 };
							 break;
	case Tetronimo::Flavor::I: positions[0] = { 1, 2 };
							 positions[1] = { 2,2 };
							 positions[2] = { 3, 2 };
							 positions[3] = { 4, 2 };
							 break;
	case Tetronimo::Flavor::O: positions[0] = { 1, 2 };
							 positions[1] = { 2, 2 };
							 positions[2] = { 2, 1 };
							 positions[3] = { 1, 1 };
							 break;
	}
#endif

	for (int i = 0; i < 4; ++i) {
#if 1
		auto* newChild = new TetrominoObject2D{ _holder };
		newChild->x = tetri[i].x();
		newChild->y = tetri[i].y();
		newChild->globalX = tetri[i].x() + tetronimi.x();
		newChild->globalY = tetri[i].y() + tetronimi.y();
#else
		_holder.addChild<Object2D>();
		auto* newChild = _holder.children().last();
		_lookup[i] = std::make_pair( *tetri, newChild );
#endif
		newChild->translate({ _xScale * tetri[i].x(), _yScale * tetri[i].y() });
		{
			using namespace Magnum::Math::Literals;
			auto* my = new RudimentarySprite(*newChild, static_cast<Magnum::Int>(16), static_cast<Magnum::Int>(16), static_cast<Magnum::Float>(1.f / 8.f), _texture, _tl, _drawables, _shader);

			auto rawColor = static_cast<std::underlying_type_t<Tetronimo::Flavor>>(tetronimi.flavor());

			my->setColor({static_cast<Magnum::UnsignedByte>(rawColor >> 24), static_cast<Magnum::UnsignedByte>(rawColor >> 16) , static_cast<Magnum::UnsignedByte>(rawColor >> 8) });
			my->setVisibility(makeVisible);
		}
		auto* newSprite = newChild->features().last();
		static_cast<RudimentarySprite*>(newSprite)->setTexCoords({ src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7] }); // CHANGE LATER!!
	}
	
	return;
}

void tetris_clone::TetronimoRendering::updateTetronimoRender(const tetris_clone::Tetronimo& tetronimi, bool rotated, tetris_clone::Tetronimo::Rotation oldRotation) noexcept {

	Magnum::Rad delta;
	{
		using namespace Magnum::Math::Literals;
		delta = (Magnum::Rad{ Magnum::Constants::piHalf() } *static_cast<std::underlying_type_t<Tetronimo::Rotation>>(tetronimi.rotation()))
			- (Magnum::Rad{ Magnum::Constants::piHalf() } *static_cast<std::underlying_type_t<Tetronimo::Rotation>>(Tetronimo::Rotation::North));
	}

	Magnum::Float translateDist = tetronimi.flavor() != tetris_clone::Tetronimo::Flavor::I ? 1.f : 2.f;
	auto intermediateTransform = Magnum::Matrix3::rotation(delta)/*;//*/ *Magnum::Matrix3::translation(Magnum::Vector2{ -translateDist, -translateDist });
	auto undoTranslate = Magnum::Matrix3::translation({ translateDist, translateDist });

	auto finishedTransform = undoTranslate * intermediateTransform;

	Magnum::Vector2 processedPoints[4];

	auto positionPtr = getPositions(tetronimi.flavor());

	auto* childPtr = dynamic_cast<TetrominoObject2D*>(_holder.children().first());

	for (int i = 0; i < 4; ++i) {

		auto vecti = tetronimi.getRotatedPositionOf(i);
		childPtr->x = vecti.first;
		childPtr->y = vecti.second;
		childPtr->globalX = vecti.first + tetronimi.x();
		childPtr->globalY = vecti.second + tetronimi.y();
		//_lookup[i] = { {vecti.first, vecti.second}, childPtr };

		auto unprocessedPoint = Magnum::Vector2{ positionPtr[i] };
		processedPoints[i] = finishedTransform.transformPoint(Magnum::Vector2{ positionPtr[i] });
		auto& processedPoint = processedPoints[i];
		processedPoints[i] = { _xScale * processedPoints[i].x(), _yScale * processedPoints[i].y() };

		auto delta = processedPoints[i] - childPtr->transformation().translation();

		childPtr->translate(delta);
		childPtr = dynamic_cast<TetrominoObject2D*>(childPtr->nextSibling());
	}

	return;
}

void tetris_clone::TetronimoRendering::updateHolder(tetris_clone::TetronimoRendering::Object2D& newHolder) {
	//_holder{ newHolder };
	return;
}
bool tetris_clone::TetronimoRendering::purgeTiles(tetris_clone::TetronimoRendering::Object2D& newOwner) {
	setTilesVisibility(false);
	for (auto* child = _holder.children().first(); child != nullptr; child = _holder.children().first()) {
		child->setParent(&newOwner);
	}
	return true;
}

void tetris_clone::TetronimoRendering::setTilesVisibility(bool isVisible) {
	for (auto& child : _holder.children()) 
		dynamic_cast<RudimentarySprite*>(child.features().first())->setVisibility(isVisible);
}

void tetris_clone::TetronimoRendering::updateColor(const tetris_clone::Tetronimo tetronimi) {
	auto col = static_cast<std::underlying_type_t<tetris_clone::Tetronimo::Flavor>>(tetronimi.flavor());

	for (auto& tile : _holder.children()) {
		dynamic_cast<RudimentarySprite*>(tile.features().first())->setColor({static_cast<Magnum::UnsignedByte>(col >> 24), static_cast<Magnum::UnsignedByte>(col >> 16) , static_cast<Magnum::UnsignedByte>(col >> 8) });
	}
}