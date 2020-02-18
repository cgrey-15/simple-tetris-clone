#include "TexturedTriangleShader.h"
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Shader.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Corrade/Containers/Reference.h>

TexturedTriangleShader::TexturedTriangleShader() {
	MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GL330);

	const Corrade::Utility::Resource rs{ "textured-triangle-data" };

	Magnum::GL::Shader vert{ Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Vertex };
	Magnum::GL::Shader frag{ Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Fragment };

	vert.addSource(rs.get("TexturedTriangleShader.vert"));
	frag.addSource(rs.get("TexturedTriangleShader.frag"));

	CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({ vert, frag }));

	attachShaders({ vert, frag });

	CORRADE_INTERNAL_ASSERT_OUTPUT(link());

	_colorUniform = uniformLocation("color");

	setUniform(uniformLocation("textureData"), TextureUnit);
}
