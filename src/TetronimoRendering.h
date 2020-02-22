#ifndef __TETRONIMO_RENDERING_H__
#define __TETRONIMO_RENDERING_H__
#include<Magnum/SceneGraph/AbstractFeature.h>
#include<Magnum/SceneGraph/AbstractTransformation.h>
#include<Magnum/Math/Vector2.h>
#include<Magnum/Shaders/Flat.h>

#include "tetromino/Tetronimo.h"

namespace tetris_clone {
	class Tetronimo;
	class TetronimoRendering : public Magnum::SceneGraph::AbstractFeature2D {
	public:

		template<typename T>
		struct TetrominoObject : public Magnum::SceneGraph::Object<T> {
			explicit TetrominoObject(Magnum::SceneGraph::Object<T>& parent) : Magnum::SceneGraph::Object<T>{ &parent } {}
			int x;
			int y;
			int globalX;
			int globalY;
		};

		typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D> Object2D;
		typedef TetrominoObject<Magnum::SceneGraph::MatrixTransformation2D> TetrominoObject2D;
#if 0
		struct TetrominoObject2D : public Object2D {
			explicit TetrominoObject2D(Object2D& parent) : Object2D{ &parent } {}
			int x;
			int y;
		};
#endif
		template<typename T> explicit TetronimoRendering(Magnum::SceneGraph::Object<T>& object, Magnum::UnsignedShort xScale, Magnum::UnsignedShort yScale, Magnum::Shaders::Flat2D& shader, Magnum::GL::Texture2D& texture, Magnum::Timeline& timeline, Magnum::SceneGraph::DrawableGroup2D& drawGroup) : 
			_holder{ object },
			Magnum::SceneGraph::AbstractFeature2D{ object },
			_transform{ object },
			_xScale{ static_cast<Magnum::Float>(xScale) },
			_yScale{ static_cast<Magnum::Float>(yScale) },
			_shader{ shader },
			_texture{ texture },
			_tl{timeline},
			_drawables{drawGroup}
		{}

		void redoTetronimoRender(Tetronimo& tetronimi, const Magnum::Vector2 * src, Magnum::Int size, bool makeVisible);
		void updateTetronimoRender(const Tetronimo& tetronimi, bool rotated = false, Tetronimo::Rotation = Tetronimo::Rotation::North) noexcept;
		void updateHolder(Object2D& newHolder);
		void updateColor(const Tetronimo tetronimi);
		void setTilesVisibility(bool isVisible);
		bool purgeTiles(Object2D& newOner);

	private:
		Object2D& _holder;
		Magnum::SceneGraph::AbstractTranslationRotation2D& _transform;
		const Magnum::Float _xScale;
		const Magnum::Float _yScale;

		Magnum::Shaders::Flat2D& _shader;
		Magnum::GL::Texture2D& _texture;
		Magnum::Timeline& _tl;
		Magnum::SceneGraph::DrawableGroup2D& _drawables;

	};
}
#endif