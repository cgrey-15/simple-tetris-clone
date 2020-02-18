#pragma once
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Color.h>
class TexturedTriangleShader : public Magnum::GL::AbstractShaderProgram {
public:
	typedef Magnum::GL::Attribute<0, Magnum::Vector2> Position;
	typedef Magnum::GL::Attribute<1, Magnum::Vector2> TextureCoordinates;

	explicit TexturedTriangleShader();

	TexturedTriangleShader& setColor(const Magnum::Color3& color) {
		setUniform(_colorUniform, color);
		return *this;
	}

	TexturedTriangleShader& bindTexture(Magnum::GL::Texture2D& texture) {
		texture.bind(TextureUnit);
		return *this;
	}

private:
	enum : Magnum::Int { TextureUnit = 0 };

	Magnum::Int _colorUniform;	

};
