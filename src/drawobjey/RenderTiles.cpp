#include "RenderTiles.h"
#include <Corrade/Corrade.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Corrade/Containers/ArrayViewStlSpan.h>
#include <span>


#if 1
drawobjey::RudimentaryRenderTiles::RudimentaryRenderTiles(const Magnum::Int width, const Magnum::Int height, const Magnum::Int rows, const Magnum::Int columns, Magnum::GL::Texture2D& texture, Object2D& object, Magnum::SceneGraph::DrawableGroup2D& group, Magnum::Shaders::Flat2D& shader, Corrade::Containers::ArrayView<Magnum::Vector2> coords) :
	Magnum::SceneGraph::Drawable2D{object, &group},
_height{height},
_width{ width },
_texture{texture},
_shader{shader},
_vao{ Corrade::Containers::DefaultInit, static_cast<size_t>(rows * columns * 4) },
_ibo{Corrade::Containers::DefaultInit, static_cast<size_t>(rows*columns*6)},
_texCoords{ Corrade::Containers::InPlaceInit, {coords[0], coords[1], coords[2], coords[3]} }
{
	bool hasUnevenRows = rows % 2;
	Magnum::Int globalHeight = rows * height;
	Magnum::Int globalWidth = columns * width;

	Magnum::Int currPosX = -(globalWidth / 2);
	Magnum::Int currPosY = (globalHeight / 2);

	Magnum::Vector2ui indices{};

	for (int i = 0; i < rows; i += 2) {
		for (int j = 0; j < columns; ++j) {
			indices += spawnTile(currPosX, currPosY, indices.x(), indices.y());
			currPosX += _width;
		}
		currPosY -= _height;
		currPosX -= width;
		for (int j = 0; j < columns; ++j) {
			indices += spawnTile(currPosX, currPosY, indices.x(), indices.y());
			currPosX -= _width;
		}
		currPosY -= _height;
		currPosX += width;
	}
	if (hasUnevenRows) {
		currPosY -= _height;
		for (int j = 0; j < columns; ++j) {
			indices += spawnTile(currPosX, currPosY, indices.x(), indices.y());
			currPosX += _width;
		}
	}
	_vertexBuffer.setData(_vao);
	_indexBuffer.setData(_ibo);

	_mesh.setCount(_ibo.size())
		.addVertexBuffer(_vertexBuffer, 0, Magnum::Shaders::Flat2D::Position{}, Magnum::Shaders::Flat2D::TextureCoordinates{})
		.setIndexBuffer(_indexBuffer, 0, Magnum::GL::MeshIndexType::UnsignedInt, 0, 64);


	//Magnum::GL::Buffer indexBuffer{};
	//_indexbuffer.setData(_ibo);

}
#endif

Magnum::Vector2ui drawobjey::RudimentaryRenderTiles::spawnTile(Magnum::Float beginPosX, Magnum::Float beginPosY, Magnum::UnsignedInt currVert, Magnum::UnsignedInt currInd)
{
	//hi
	auto* vertPtr = _vao.data();
	vertPtr[currVert] = TriangleVertex{ {beginPosX, beginPosY-_height}, _texCoords[0] };
	vertPtr[currVert + 1] = TriangleVertex{ {beginPosX + _width, beginPosY - _height}, _texCoords[1] };
	vertPtr[currVert + 2] = TriangleVertex{ {beginPosX + _width, beginPosY}, _texCoords[2] };
	vertPtr[currVert + 3] = TriangleVertex{ {beginPosX, beginPosY}, _texCoords[3] };

	TriangleVertex something[4] = { vertPtr[currVert], vertPtr[currVert + 1], vertPtr[currVert + 2], vertPtr[currVert + 3] };

	auto* indPtr = _ibo.data();

	indPtr[currInd] = currVert;
	indPtr[currInd + 1] = currVert + 2;
	indPtr[currInd + 2] = currVert + 3;
	indPtr[currInd + 3] = currVert;
	indPtr[currInd + 4] = currVert + 1;
	indPtr[currInd + 5] = currVert + 2;

	return Magnum::Vector2ui{4, 6};
}

void drawobjey::RudimentaryRenderTiles::draw(const Magnum::Matrix3& transform, Magnum::SceneGraph::Camera2D& camera)
{
	_shader.bindTexture(_texture);
	_shader.setTransformationProjectionMatrix(camera.projectionMatrix() * transform);
	_mesh.draw(_shader);

}
drawobjey::RudimentaryRenderTiles& drawobjey::RudimentaryRenderTiles::setTexCoords(Corrade::Containers::StaticArrayView<4, Magnum::Vector2> data) {
	for (int i = 0; i < data.size(); ++i) {
		_texCoords[i] = data[i];
	}

	for (int i = 0; i < _vao.size(); i += 4) {
		_vao[i].texCoord = _texCoords[0];
		_vao[i + 1].texCoord = _texCoords[1];
		_vao[i + 2].texCoord = _texCoords[2];
		_vao[i + 3].texCoord = _texCoords[3];
	}
	std::vector<TriangleVertex> hinder{ _vao.cbegin(), _vao.cend() };
	//for(int i = 0; i < _height)

	_vertexBuffer.setSubData(0, _vao);

	return *this;
}
