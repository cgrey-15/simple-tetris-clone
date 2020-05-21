#ifndef SPRITE_HANDLER_H
#define SPRITE_HANDLER_H

#include <tuple>
#include <map>
#include <Corrade/Containers/Array.h>
#include <Magnum/Magnum.h>
#include <Corrade/PluginManager/Manager.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Corrade/Utility/Resource.h>
#include "support/Common.h"
#include "support/ResourceProfile.h"
#include "drawobjey/Sprite.h"

namespace Magnum {
	namespace Trade {
		class AbstractImporter;
	}
}

namespace drawobjey {
	class RudimentaryRenderTiles;
	class RudimentarySprite;

	struct SpriteProfile {
		resource::RsrcId id;
	};

	class SpriteHandler {
		using   RsrcPtrTableT =              std::pair< resource::RsrcId, size_t >;
		using RsrcSizesTableT =              std::pair< resource::RsrcId, size_t >;
		using VertPosMappingT = std::pair< std::pair<uint16_t, uint16_t>, uint16_t >; 
		using IdxBufMappingT = std::pair<std::pair<uint16_t, uint16_t>, uint16_t>;
		using TextureLookupTableT = std::pair<resource::RsrcId, Magnum::GL::Texture2D*>;
		using QuadPtrLookupT = std::pair<resource::RsrcId, uint16_t>;

		struct UVPoint {
			Magnum::Float u;
			Magnum::Float v;
		};
		struct VertexPos2D {
			Magnum::Float x;
			Magnum::Float Y;
		};

		struct sillyclass { size_t me; resource::RsrcId id; };
		static constexpr auto value = sizeof(std::pair<resource::RsrcId, uint16_t>);

		static constexpr float SPRITE_HANDLER_SCALE_FACTOR = 0.45f;
		Corrade::Containers::Array<UVPoint>         uvPositions_;
		Corrade::Containers::Array<VertexPos2D> vertexPositions_;
		Magnum::GL::Buffer vertBuffer_;
		size_t vertBufferSize_;

		Corrade::Containers::Array<RsrcPtrTableT>   uvPosPtr_;
		Corrade::Containers::Array<RsrcPtrTableT> vertPosPtr_;

		Corrade::Containers::Array<RsrcSizesTableT>   uvPosCount_;
		Corrade::Containers::Array<RsrcSizesTableT> vertPosCount_;


		Corrade::Containers::Array<Magnum::GL::Texture2D*> textures_;
		Corrade::Containers::Array<TextureLookupTableT> textureIndex_;
		std::map<std::string, Magnum::GL::Texture2D> texchures_;

		Corrade::Containers::Array<VertPosMappingT> quadDimensions_;
		Corrade::Containers::Array<QuadPtrLookupT> assetQuads_;
		Magnum::GL::Buffer indicesBuffer_;

		Corrade::Containers::Array<uint8_t>          vertexIndices_;
		Corrade::Containers::Array<RsrcPtrTableT> vertexIndicesPtr_;
		const static int         indexOrderSize_;
		const static unsigned char indexOrder_[];
		//size_t texturesSize_;

		//Corrade::Utility::Resource& rsrcMngr_;
	public:
		using AttributeElementCountT = int;
		const static AttributeElementCountT uvPositionsPerFrame;
		const static AttributeElementCountT vertPositionsPerFrame;

		//explicit SpriteHandler(Corrade::Utility::Resource& manager);
		SpriteHandler();

		bool makeWholeFromFile(resource::RsrcId id, const resource::ResourceMgr& mgr, const Corrade::Utility::Resource& resources, Magnum::Trade::AbstractImporter& ip);
		void setUpBackground(resource::RsrcId id, drawobjey::RudimentaryRenderTiles& tiles);

		auto getTexture(const resource::resource_profile& profile, const resource::ResourceMgr& mgr) -> Magnum::GL::Texture2D&;
		auto getTexture(const resource::RsrcId id                                                  ) -> Magnum::GL::Texture2D&;

		struct BufferInformation {
			Magnum::GL::Buffer* const buffer;
			const size_t              offset;
		};

		auto getDimensions(const resource::RsrcId id)->std::pair<int, int>;
		auto getVertexBuffer(const resource::RsrcId id) -> BufferInformation;
		auto getIndicesBuffer(const resource::RsrcId id) -> BufferInformation;
		auto getTexCoordData(const resource::RsrcId id) -> Corrade::Containers::ArrayView<const Magnum::Vector2>;
		auto getIndicesBuffer() -> std::pair<const unsigned char*, size_t> { return {indexOrder_, indexOrderSize_}; }

	private:
		static void fillBuffer_(Magnum::GL::Buffer& dstBff, const size_t bufferSize, const Corrade::Containers::Array<VertexPos2D>& src, const size_t index, const size_t elementAmt);
		template<typename Ty, int N>
		static void insert4Elements(Magnum::Containers::Array<Ty>& positions, Ty(&&elements)[N]);
		template<typename Ty>
		static void syncBufferVertexPositions(Magnum::GL::Buffer&, Corrade::Containers::Array<Ty>& vertices, const size_t insertedIndex, const size_t sizeInserted);
	};
}
#endif // SPRITE_HANDLER_H
