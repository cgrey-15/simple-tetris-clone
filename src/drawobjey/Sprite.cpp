#include "Sprite.h"
#include <Magnum/Timeline.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Math/FunctionsBatch.h>

drawobjey::RudimentarySprite::RudimentarySprite(Object2D& object, Magnum::Int width, Magnum::Int height, Magnum::Float frameDuration, Magnum::GL::Texture2D& texture, Magnum::Timeline& timeline,Magnum::SceneGraph::DrawableGroup2D& group, Magnum::Shaders::Flat2D& shader, bool useNewMethod ) :
	Magnum::SceneGraph::Drawable2D{object, &group},
	useNewMethod_{useNewMethod},
	_texture{ texture },
	_color{ 0xff, 0xff, 0xff },
	_tl{ timeline },
	_shader{ shader },
	_vertexBuffer{},
	_texCoords{},
	_prevUpdatePoint{},
	_frameDur{frameDuration},
	_size{},
	_currFrame{},
	_visible{true}
{

	_vao[0] = { {-0.5f, -0.5f}, {  0.0f       , (1.f - (16.f / 64.f))} };//0.0f}},
	_vao[1] = { { 0.5f, -0.5f}, {(16.f / 64.f), (1.f - (16.f / 64.f))} };//1.0f, 0.0f}},
	_vao[2] = { { 0.5f,  0.5f}, {(16.f / 64.f)/*1.0f*/, 1.0f} };
	_vao[3] = { {-0.5f,  0.5f}, { 0.0f,                 1.0f} };

	auto whalf{ static_cast<Magnum::Float>(width/2) };
	auto hhalf{ static_cast<Magnum::Float>(height/2) };

#if 0
	_vao[0].position = { -whalf, -hhalf };
	_vao[1].position = { whalf, -hhalf };
	_vao[2].position = { whalf, hhalf };
	_vao[3].position = { -whalf, hhalf };
#else
	_vao[0].position = { 0.f, 0.f };
	_vao[1].position = { 0.f, static_cast<Magnum::Float>(width) };
	_vao[2].position = { static_cast<Magnum::Float>(height), static_cast<Magnum::Float>(width) };
	_vao[3].position = { static_cast<Magnum::Float>(height), 0.f };

#endif

	_vertexBuffer.setData(Corrade::Containers::ArrayView<TriangleVertex>(_vao, 4));

	const char indyData[] = { 0, 2, 3, 0, 1, 2 };

	Magnum::GL::Buffer indexBuffer{};
	indexBuffer.setData(indyData);

	Magnum::UnsignedInt inStart = 0, inEnd = 5;
	Magnum::GL::MeshIndexType inType{ Magnum::GL::MeshIndexType::UnsignedByte };

	_mesh.setCount(6)
		.addVertexBuffer(_vertexBuffer,
		0,
			Magnum::Shaders::Flat2D::Position{},
			Magnum::Shaders::Flat2D::TextureCoordinates{})
		//TexturedTriangleShader::Position{},
		//TexturedTriangleShader::TextureCoordinates{})
		.setIndexBuffer(std::move(indexBuffer), 0, inType, inStart, inEnd);
}

drawobjey::RudimentarySprite::RudimentarySprite(Object2D& object,
	Magnum::Int w,
	Magnum::Int h,
	Magnum::Float seqDelta,
	Magnum::GL::Texture2D& txtr,
	size_t vertBuffiPtr,
	size_t idxBuffiPtr,
	Magnum::GL::Buffer& vertBuffi,
	Corrade::Containers::ArrayView<const Magnum::Vector2> texCoords,
	Magnum::GL::Buffer& idxBuffi,
	Magnum::Timeline& tl,
	Magnum::SceneGraph::DrawableGroup2D& group,
	Magnum::Shaders::Flat2D& shader,
	bool useNewMethod
) :
	Magnum::SceneGraph::Drawable2D{object, &group},
	_size{},
	_currFrame{},
	_frameDur{seqDelta},
	_prevUpdatePoint{},
	_visible{},
	useNewMethod_{useNewMethod},
	_tl{tl},
	_texture{txtr},
	_color{0xff, 0xff, 0xff},
	_shader{ shader },
	_mesh{},
	_vertexBuffer{},
	_texCoordBuffer{},
	texCoordData_{texCoords}
{
	_texCoordBuffer.setData(texCoordData_.slice(0, 4));
	size_t uvPtr = 0;
	_mesh.setCount(6)
		.addVertexBuffer(vertBuffi, vertBuffiPtr, Magnum::Shaders::Flat2D::Position{})
		.addVertexBuffer(_texCoordBuffer, uvPtr, Magnum::Shaders::Flat2D::TextureCoordinates{})
		.setIndexBuffer(idxBuffi, idxBuffiPtr, Magnum::GL::MeshIndexType::UnsignedByte, vertBuffiPtr, vertBuffiPtr + 6);
}

#if 0
, RudimentarySprite::, RudimentarySprite(, RudimentarySprite&& src) : Magnum::SceneGraph::Drawable2D{src.object(), src.drawables()}, _tl{ src._tl }, _texture{ src._texture }, _shader{ src._shader } {

}
#endif

void drawobjey::RudimentarySprite::getPrevTime() {
	_prevUpdatePoint = _tl.previousFrameTime();
}

void drawobjey::RudimentarySprite::setColor(Magnum::Color3ub color)
{
	_color = color;
}

drawobjey::RudimentarySprite& drawobjey::RudimentarySprite::setVisibility(const bool isVisible) {
	_visible = isVisible;
	return *this;
}

drawobjey::RudimentarySprite& drawobjey::RudimentarySprite::setTexCoords(const std::initializer_list<Magnum::Vector2>& list) {
	_texCoords = Corrade::Containers::Array<Magnum::Vector2>(Corrade::Containers::InPlaceInit, list);
	_size = _texCoords.size();
	return *this;
}

drawobjey::RudimentarySprite& drawobjey::RudimentarySprite::setTexCoords(Corrade::Containers::Array<Magnum::Vector2>&& vertices) {
	_texCoords = Corrade::Containers::Array<Magnum::Vector2>(std::move(vertices));
	_size = _texCoords.size();
	return *this;
}

drawobjey::RudimentarySprite& drawobjey::RudimentarySprite::setTexCoords(Magnum::Vector2 singles) {
	_texCoords = Corrade::Containers::Array<Magnum::Vector2>(Corrade::Containers::InPlaceInit, {singles});
	_size = _texCoords.size();
	return *this;
}

drawobjey::RudimentarySprite& drawobjey::RudimentarySprite::setTexCoords(const Magnum::Vector2* data, size_t frameCount) {
	using namespace Corrade;

	texCoordData_ = Containers::ArrayView<const Magnum::Vector2>(data, frameCount * 4);

	return *this;
}

void drawobjey::RudimentarySprite::draw(const Magnum::Matrix3& transform, Magnum::SceneGraph::Camera2D& camera) {
	using namespace Magnum::Math::Literals;
	_shader.setColor({Magnum::Math::unpack<Magnum::Color3>(_color)})
		.bindTexture(_texture);

	auto elapsed_time = _tl.previousFrameTime() + _tl.previousFrameDuration();
	if (elapsed_time > _prevUpdatePoint + _frameDur) {

		
		_currFrame = ++_currFrame % 8;
		/* TODO: Add your drawing code here */
		if (!useNewMethod_) {
			_vao[0].texCoord = { _texCoords[_currFrame][0],
										   (1.f - _texCoords[_currFrame][1]) - (16.f / 64.f) };
			_vao[1].texCoord = { _texCoords[_currFrame][0] + (16.f / 64.f),
										   (1.f - _texCoords[_currFrame][1]) - (16.f / 64.f) };
			_vao[2].texCoord = { _texCoords[_currFrame][0] + (16.f / 64.f),
										   (1.f - _texCoords[_currFrame][1]) };
			_vao[3].texCoord = { _texCoords[_currFrame][0],
										   (1.f - _texCoords[_currFrame][1]) };

			_vertexBuffer.setSubData(0, Magnum::Containers::ArrayView<TriangleVertex>(_vao, 4));
		}
		else {
			_texCoordBuffer.setSubData(0, texCoordData_.slice<4>(4*_currFrame));
		}
		_prevUpdatePoint = elapsed_time;// - (_lastUpdate + _deltaFrameDuration);
		
	}

	if (_visible) {
		_shader.setTransformationProjectionMatrix(camera.projectionMatrix() * transform);
		_mesh.draw(_shader);
	}
	_shader.setColor(0xffffffff_rgbaf);

}
