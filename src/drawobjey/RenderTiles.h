#ifndef __TILES_H__
#define __TILES_H__

#include <Corrade/Containers/Array.h>

#include <Magnum/Math/Vector2.h>

#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
//#include <Magnum/SceneGraph/DualComplexTransformation.h>
#include <Magnum/SceneGraph/Scene.h>

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Mesh.h>

#include <Magnum/Shaders/Flat.h>
//#include "TexturedTriangleShader.h"


namespace drawobjey
{
	class RudimentaryRenderTiles : public Magnum::SceneGraph::Drawable2D {
	public:
		typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D> Object2D;

		//RudimentarySprite(Object2D& obj) : Magnum::SceneGraph::Drawable2D{ obj }, _texture{}, _shader{}, _tl{}{}
		explicit RudimentaryRenderTiles(const Magnum::Int width, const Magnum::Int height, const Magnum::Int rows, const Magnum::Int columns, Magnum::GL::Texture2D& texture, Object2D& object, Magnum::SceneGraph::DrawableGroup2D& group, Magnum::Shaders::Flat2D& shader, Corrade::Containers::ArrayView<Magnum::Vector2> coords);// :
#if 0
		Magnum::SceneGraph::Drawable2D{ object, &group },
			_texture{ texture },
			_tl{ timeline },
			_shader{ shader },
			_vertexBuffer{},
			_texCoords{},
			_prevUpdatePoint{},
			_frameDur{ frameDuration },
			_size{},
			_currFrame{};
#endif

		//RudimentarySprite(RudimentarySprite&& other);// : Magnum::SceneGraph::Drawable2D{ other.object(), other.drawables() }, _tl{ other._tl }, _texture{ other._texture }, _shader{ other._shader };
		//RudimentarySprite& operator=(RudimentarySprite&& other);

		RudimentaryRenderTiles& setTexCoords(Corrade::Containers::StaticArrayView<4, Magnum::Vector2> data);
#if 0
		RudimentaryRenderTiles& setTexCoords(const std::initializer_list<Magnum::Vector2>& texCoords);
		RudimentaryRenderTiles& setTexCoords(Corrade::Containers::Array<Magnum::Vector2>&& texCoords);
		RudimentaryRenderTiles& setTexCoords(Magnum::Vector2 single);
#endif

		//void getPrevTime();
		//void initTime();

	private:

		const Magnum::Int _height;
		const Magnum::Int _width;

		struct TriangleVertex {
			Magnum::Vector2 position;
			Magnum::Vector2 texCoord;
		};

		void draw(const Magnum::Matrix3& transform, Magnum::SceneGraph::Camera2D& camera);
		Magnum::Vector2ui spawnTile(Magnum::Float beginPosX, Magnum::Float beginPosY, Magnum::UnsignedInt currVert, Magnum::UnsignedInt currInd);

		Magnum::GL::Texture2D& _texture;
		//TexturedTriangleShader&  _shader;
		Magnum::Shaders::Flat2D& _shader;

		Magnum::GL::Mesh           _mesh;
		Magnum::GL::Buffer _vertexBuffer;
		Magnum::GL::Buffer _indexBuffer;

		TriangleVertex _vaoIndices[4];
		Corrade::Containers::Array<TriangleVertex> _vao;
		Corrade::Containers::Array<Magnum::Int> _ibo;

		Corrade::Containers::Array<Magnum::Vector2> _texCoords;
	};
}

#endif
