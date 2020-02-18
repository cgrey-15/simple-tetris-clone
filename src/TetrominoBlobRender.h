#ifndef __TETROMINO_BLOB_RENDER_H__
#define __TETRONIMO_BLOB_RENDER_H__

#include "Sprite.h"
#include<Magnum/SceneGraph/Drawable.h>
#include<Magnum/GL/Mesh.h>
#include<Corrade/Containers/Array.h>
#include<Corrade/Containers/ArrayView.h>

namespace tetris_clone {
	class TetrominoRenderBlob : public Magnum::SceneGraph::Drawable2D {
	public:
		explicit TetrominoRenderBlob(RudimentarySprite::Object2D& holder, RudimentarySprite&& old);
		void draw(const Magnum::Matrix3& transform, Magnum::SceneGraph::Camera2D& camera) override;
	private:
		friend void take(RudimentarySprite&& victim);
		size_t _meshSize;
		size_t _GLBufferSize;
		Corrade::Containers::Array<Magnum::GL::Mesh> _staticMeshes;
		Corrade::Containers::ArrayView<Magnum::GL::Buffer> _buffies;
	};
}

#endif