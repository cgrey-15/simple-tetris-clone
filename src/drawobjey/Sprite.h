#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <Corrade/Containers/Array.h>

#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Color.h>

#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/Scene.h>

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Mesh.h>

#include "Magnum/Shaders/Flat.h"


namespace drawobjey
{
	class RudimentarySprite : public Magnum::SceneGraph::Drawable2D {
	public:
		typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D> Object2D;

		explicit RudimentarySprite(Object2D& object, Magnum::Int width, Magnum::Int height, Magnum::Float frameDuration, Magnum::GL::Texture2D& texture, Magnum::Timeline& timeline, Magnum::SceneGraph::DrawableGroup2D& group, Magnum::Shaders::Flat2D& shader, bool useNewMethod=false);// :
		RudimentarySprite(Object2D& object, Magnum::Int wPx, Magnum::Int hPx, Magnum::Float seqDelta, Magnum::GL::Texture2D& txtr, size_t vertBuffPtr, size_t idxBuffPtr, Magnum::GL::Buffer& vertBuffi, Corrade::Containers::ArrayView<const Magnum::Vector2> texCoords, Magnum::GL::Buffer& idxBuffi, Magnum::Timeline& tl, Magnum::SceneGraph::DrawableGroup2D& group, Magnum::Shaders::Flat2D& shader, bool useNewMethod = true);


		RudimentarySprite& setTexCoords(const std::initializer_list<Magnum::Vector2>& texCoords);
		RudimentarySprite& setTexCoords(Corrade::Containers::Array<Magnum::Vector2>&& texCoords);
		RudimentarySprite& setTexCoords(const Magnum::Vector2* data, size_t frameCount);
		RudimentarySprite& setTexCoords(Magnum::Vector2 single);
		RudimentarySprite& setVisibility(const bool isVisible);

		void getPrevTime();

		void setColor(Magnum::Color3ub color);
		//void initTime();

	private:
		void draw(const Magnum::Matrix3& transform, Magnum::SceneGraph::Camera2D& camera);

		struct TriangleVertex {
			Magnum::Vector2 position;
			Magnum::Vector2 texCoord;
		};

		Magnum::Int              _size;
		Magnum::Int         _currFrame;
		Magnum::Float        _frameDur;
		Magnum::Float _prevUpdatePoint;

		bool _visible;
		bool useNewMethod_;

		const Magnum::Timeline& _tl;

		Magnum::GL::Texture2D& _texture;
		Magnum::Color3ub          _color;
		Magnum::Shaders::Flat2D& _shader;

		Magnum::GL::Mesh           _mesh;
		Magnum::GL::Buffer _vertexBuffer;
		Magnum::GL::Buffer _texCoordBuffer;

#if 1
		TriangleVertex _vao[4];
		Corrade::Containers::ArrayView<TriangleVertex> _vaoView;
		Corrade::Containers::ArrayView<const char> _iboView;

		Corrade::Containers::Array<Magnum::Vector2> _texCoords;
		Corrade::Containers::ArrayView<const Magnum::Vector2> texCoordData_;
		Corrade::Containers::ArrayView<Magnum::Vector2> vertPosData_;
#endif
	};
}

#endif