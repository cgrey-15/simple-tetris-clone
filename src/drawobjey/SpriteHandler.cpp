#include <Corrade/Utility/Resource.h>
#include <Corrade/PluginManager/Manager.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/ImageView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Magnum/GL/TextureFormat.h>
#include <nlohmann/json.hpp>
#include <regex>
#include "support/ResourceProfile.h"
#include "drawobjey/SpriteHandler.h"
#include "drawobjey/Sprite.h"
#include "drawobjey/RenderTiles.h"
#include "support/Utilities.hpp"

const drawobjey::SpriteHandler::AttributeElementCountT drawobjey::SpriteHandler::uvPositionsPerFrame = 4;
const drawobjey::SpriteHandler::AttributeElementCountT drawobjey::SpriteHandler::vertPositionsPerFrame = 4;

const int drawobjey::SpriteHandler::indexOrderSize_ = 6;
const unsigned char drawobjey::SpriteHandler::indexOrder_[drawobjey::SpriteHandler::indexOrderSize_] = {0, 2, 3, 0, 1, 2};


//drawobjey::SpriteHandler::SpriteHandler(Corrade::Utility::Resource& manager) : rsrcMngr_{ manager } {}
drawobjey::SpriteHandler::SpriteHandler() :
	uvPositions_{},
	uvPosPtr_{},
	uvPosCount_{},
	vertexPositions_{},
	vertPosPtr_{},
	vertPosCount_{},
	vertBuffer_{Magnum::GL::Buffer::TargetHint::Array},
	vertBufferSize_{},
	texchures_{},
	textures_{},
	textureIndex_{},
	quadDimensions_{},
	vertexIndices_{},
	vertexIndicesPtr_{},
	indicesBuffer_{ Magnum::GL::Buffer::TargetHint::ElementArray, Corrade::Containers::StaticArrayView<indexOrderSize_, const unsigned char>{indexOrder_} }
{}

bool drawobjey::SpriteHandler::makeWholeFromFile(resource::RsrcId id, const resource::ResourceMgr& mgr, const Corrade::Utility::Resource& rs, Magnum::Trade::AbstractImporter& ip) {
	using stdstr = std::string;
	using stdre = std::regex;
	// load json first
	auto profile = mgr[id];

	const char* packname = mgr.getPackgroupWithProfile(profile);

	// load and import tga image

#if 0
	Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> manager{};

	Corrade::Containers::Pointer<Magnum::Trade::AbstractImporter> imgImporter = manager.loadAndInstantiate("TgaImporter");
	if (!imgImporter) {
		std::exit(-4);
	}
#endif

	if (texchures_.find(packname) == texchures_.end()) {
		stdstr tgaName = stdstr{ packname } + ".tga";

		if (ip.isOpened()) {
			ip.close();
		}
		if (!ip.openData(rs.getRaw(tgaName.c_str()))) {
			std::exit(-5);
		}
		std::vector<std::string> huh{ 10 };

		//auto someImg = ip.image2DName(0);
		//auto imgId = ip.image2DForName(tgaName.c_str());
		Magnum::Containers::Optional<Magnum::Trade::ImageData2D> imgRawData = ip.image2D(0);
		CORRADE_INTERNAL_ASSERT(imgRawData);

		auto insertOutcome = texchures_.emplace(std::string{ packname }, Magnum::GL::Texture2D{});
		if (insertOutcome.second != true) {
			std::exit(22);
		}
		auto texItr = insertOutcome.first;

		//Magnum::GL::Texture2D* texItr = new Magnum::GL::Texture2D{};
		texItr->second.setWrapping(Magnum::GL::SamplerWrapping::ClampToEdge)
			.setMagnificationFilter(Magnum::GL::SamplerFilter::Nearest)
			.setMinificationFilter(Magnum::GL::SamplerFilter::Nearest)
			.setStorage(1, Magnum::GL::TextureFormat::RGBA8, { imgRawData->size() })
			.setSubImage(0, { 0, 0 }, *imgRawData);

#if 0
		Corrade::Containers::arrayAppend(textures_, { texture });
		Corrade::Containers::arrayAppend(textureIndex_, { id, textures_.size() - 1 });
#endif
	}

	Corrade::Containers::arrayAppend(textureIndex_, TextureLookupTableT{ id, &texchures_.find(packname)->second });


	stdstr jsonName = stdstr{ packname } + ".json";

	auto jsonThingy = rs.getRaw(jsonName.c_str());
	auto jsonDoc = nlohmann::json::parse(jsonThingy);


	std::regex nameRx{ "([A-Za-z][A-Za-z0-9_]*)(::([A-Za-z][A-Za-z0-9_]*))*" };
	std::cmatch rxResults{};
	std::string spriteLabel{};
	auto realName = mgr.getNameWithProfile(profile);
	if (std::regex_match(realName, rxResults, nameRx)) {
		auto result = rxResults[rxResults.size() - 1];
		spriteLabel = { result.first, result.second };
	}
	else {
		std::exit(-50);
	}

	auto itEl = std::find_if(jsonDoc.begin(), jsonDoc.end(), [&spriteLabel](const nlohmann::json& obj) {return obj.find("label")->get<std::string>() == spriteLabel; });

	{
		uint16_t h = (*itEl)["dimension"]["height"];
		uint16_t w = (*itEl)["dimension"]["width"];
		auto dimKey = std::make_pair(h, w);

		auto vertPosLocation = std::find_if(std::begin(quadDimensions_), std::end(quadDimensions_), 
			[h, w](const VertPosMappingT v) {return v.first.first == h && v.first.second == w; });

		if (!vertPosLocation) { // not sure if expression behaves as expected
			Corrade::Containers::arrayAppend(quadDimensions_, { {h, w}, static_cast<uint16_t>(vertexPositions_.size()) });
			vertPosLocation = &quadDimensions_.back();
			float fh = static_cast<int>(h);
			float fw = static_cast<int>(w);
			//insert4Elements()
			insert4Elements(vertexPositions_, { {0.f, 0.f}, {fw, 0.f}, {fw, fh}, {0.f, fh} });
#if 0
			Corrade::Containers::arrayAppend(vertexPositions_, { 0.f, 0.f });
			Corrade::Containers::arrayAppend(vertexPositions_, { 0.f, fh });
			Corrade::Containers::arrayAppend(vertexPositions_, { fw, fh });
			Corrade::Containers::arrayAppend(vertexPositions_, { fw, 0.f });
#endif
			syncBufferVertexPositions(vertBuffer_, vertexPositions_, vertPosLocation->second, 4);

			auto insertedIndexIndicesPtr = vertexIndices_.size();
			insert4Elements(vertexIndices_, { static_cast<uint8_t>(vertPosLocation->second + indexOrder_[0]),
											 static_cast<uint8_t>(vertPosLocation->second + indexOrder_[1]),
											 static_cast<uint8_t>(vertPosLocation->second + indexOrder_[2]),
											 static_cast<uint8_t>(vertPosLocation->second + indexOrder_[3]),
											 static_cast<uint8_t>(vertPosLocation->second + indexOrder_[4]),
											 static_cast<uint8_t>(vertPosLocation->second + indexOrder_[5])
				});
			syncBufferVertexPositions(indicesBuffer_, vertexIndices_, insertedIndexIndicesPtr, 6);
			Corrade::Containers::arrayAppend(vertexIndicesPtr_, { id, insertedIndexIndicesPtr });
		}

		size_t newPtr = vertPosLocation->second;

		Corrade::Containers::arrayAppend(assetQuads_, { id, vertPosLocation - quadDimensions_.cbegin() });
		Corrade::Containers::arrayAppend(vertPosPtr_, { id, newPtr });
		Corrade::Containers::arrayAppend(vertPosCount_, {id, 4});
	}

	nlohmann::json& texCoords = (*itEl)["frames"];

	{

		size_t newUVPtr = uvPositions_.size();
		size_t length{};
		for (auto& uvCoord : texCoords) {
			UVPoint minCoord = { uvCoord["minimum-uv"]["u"], uvCoord["minimum-uv"]["v"] };
			UVPoint maxCoord = { uvCoord["maximum-uv"]["u"], uvCoord["maximum-uv"]["v"] };


			Corrade::Containers::arrayAppend(uvPositions_, {minCoord.u, minCoord.v});
			Corrade::Containers::arrayAppend(uvPositions_, {maxCoord.u, minCoord.v});
			Corrade::Containers::arrayAppend(uvPositions_, {maxCoord.u, maxCoord.v});
			Corrade::Containers::arrayAppend(uvPositions_, {minCoord.u, maxCoord.v});

			length += 4;
		}
		Corrade::Containers::arrayAppend(uvPosPtr_, { id, newUVPtr });
		Corrade::Containers::arrayAppend(uvPosCount_, { id, length });
	}


	return true;
}

void drawobjey::SpriteHandler::setUpBackground(resource::RsrcId id, drawobjey::RudimentaryRenderTiles& tiles)
{
	constexpr Magnum::Vector2 p[4] =
	{
		supp::supporianCoordsUnconvert<128, 128>(48, 16),
		supp::supporianCoordsUnconvert<128, 128>(64, 16),
		supp::supporianCoordsUnconvert<128, 128>(64, 0),
		supp::supporianCoordsUnconvert<128, 128>(48, 0)
	};

	UVPoint hey[4] = { {p[0].x(), p[0].y()},
					   {p[1].x(), p[1].y()},
					   {p[2].x(), p[2].y()},
					   {p[3].x(), p[3].y()}
	};

	auto it = std::find_if(std::begin(uvPosPtr_), std::end(uvPosPtr_), [id](const decltype(uvPosPtr_)::Type& v) {return v.first == id; });
	if (it != std::end(uvPosPtr_)) {
		std::vector<Magnum::Vector2> hummus{ reinterpret_cast<Magnum::Vector2*>(uvPositions_.data()) + it->second, reinterpret_cast<Magnum::Vector2*>(uvPositions_.data()) + it->second + 4 };
#if 0
		tiles.setTexCoords(Corrade::Containers::StaticArrayView<4, Magnum::Vector2>{ reinterpret_cast<Magnum::Vector2*>(uvPositions_.data()/*hey*/) + it->second });
#else
		tiles.setTexCoords(Corrade::Containers::StaticArrayView<4, Magnum::Vector2>{ reinterpret_cast<Magnum::Vector2*>(hey) });

#endif
	}
	else {
		std::exit(25);
	}
	return;
}

#if 0
drawobjey::RudimentarySprite drawobjey::SpriteHandler::setUpSprite(const resource::RsrcId id, const resource::ResourceMgr& mgr, RudimentarySprite::Object2D& obj, int w, int h) {
	using namespace Magnum;

	auto profile = mgr[id];

	const char* pack = mgr.getPackgroupWithProfile(profile);

	GL::Texture2D& texturi = texchures_[pack];

	return RudimentarySprite(obj,w, h, 0.f, texturi,  )

}
#endif

Magnum::GL::Texture2D& drawobjey::SpriteHandler::getTexture(const resource::resource_profile& profile, const resource::ResourceMgr& mgr) {
	return texchures_[mgr.getPackgroupWithProfile(profile)];
}

Magnum::GL::Texture2D& drawobjey::SpriteHandler::getTexture(const resource::RsrcId id) {
	return *std::find_if(textureIndex_.cbegin(), textureIndex_.cend(), [id](const TextureLookupTableT v) {return v.first == id; })->second;
}


drawobjey::SpriteHandler::BufferInformation drawobjey::SpriteHandler::getVertexBuffer(const resource::RsrcId id) {
	return{ &vertBuffer_, std::find_if(vertPosPtr_.begin(), vertPosPtr_.end(), [id](const RsrcPtrTableT v) {return v.first == id; })->second*sizeof(VertexPos2D) };
}

auto drawobjey::SpriteHandler::getDimensions(const resource::RsrcId id) -> std::pair<int, int> {
	auto [h, w] = quadDimensions_[std::find_if(assetQuads_.cbegin(), assetQuads_.cend(), [id](const QuadPtrLookupT v) {return v.first == id; })->second].first;
	return { h, w };
}

Corrade::Containers::ArrayView<const Magnum::Vector2> drawobjey::SpriteHandler::getTexCoordData(const resource::RsrcId id) {
	auto offset = std::find_if(uvPosPtr_.cbegin(), uvPosPtr_.cend(), [id](const RsrcPtrTableT v) {return v.first == id; })->second;
	auto size = std::find_if(uvPosCount_.cbegin(), uvPosCount_.cend(), [id](const RsrcPtrTableT v) {return v.first == id; })->second;
	return Corrade::Containers::ArrayView<const Magnum::Vector2>{reinterpret_cast<Magnum::Vector2*>(&uvPositions_[offset]), size };
}
drawobjey::SpriteHandler::BufferInformation drawobjey::SpriteHandler::getIndicesBuffer(const resource::RsrcId id) {
	return{ &indicesBuffer_, std::find_if(vertexIndicesPtr_.begin(), vertexIndicesPtr_.end(), [id](const RsrcPtrTableT v) {return v.first == id; })->second*sizeof(uint8_t) };
}

void drawobjey::SpriteHandler::fillBuffer_(Magnum::GL::Buffer& dstBff, const size_t bufferSize, const Corrade::Containers::Array<VertexPos2D>& src, const size_t index, const size_t elementAmt) {

	//size_t updatedBufferSize{};

#if 0
	size_t computedSize = (src.size()*sizeof(VertexPos2D)) + (elementAmt * sizeof(VertexPos2D));

	if (computedSize > bufferSize) {
		size_t iThBufferSize = bufferSize;
		while (iThBufferSize < computedSize) {
			iThBufferSize += static_cast<size_t>(static_cast<double>(bufferSize) * static_cast<double>(SPRITE_HANDLER_SCALE_FACTOR));
		}
		size_t updatedBufferSize = iThBufferSize;

		dstBff.setData(src);
	}
	else {
		Corrade::Containers::ArrayView<const VertexPos2D> view{ &src[index], elementAmt };
		dstBff.setSubData(index * sizeof(VertexPos2D), view);
	}
#endif

}

template<typename Ty, int N>
void drawobjey::SpriteHandler::insert4Elements(Corrade::Containers::Array<Ty>& positions, Ty(&& elements)[N]) {
	auto cap = Corrade::Containers::arrayCapacity(positions);
	bool tampered{};
	size_t updatedCap = cap;
	while (! ((positions.size() + 4) > cap)) {
		tampered = true;
		updatedCap = updatedCap + static_cast<size_t>(std::ceil(static_cast<double>(cap) * static_cast<double>(SPRITE_HANDLER_SCALE_FACTOR)));
	}

	if (tampered) {
		Corrade::Containers::arrayResize(positions, updatedCap);
	}
	for (int i = 0; i < N; ++i) {
		Corrade::Containers::arrayAppend(positions, std::forward<Ty>(elements[i]));
	}
}

template<typename Ty>
void drawobjey::SpriteHandler::syncBufferVertexPositions(Magnum::GL::Buffer& vertexPosBuffer, Corrade::Containers::Array<Ty>& vertices, const size_t insertedIndex, const size_t sizeInserted) {
	if (vertexPosBuffer.size() != (Corrade::Containers::arrayCapacity(vertices)*sizeof(Ty))) {
		size_t capacity = Corrade::Containers::arrayCapacity(vertices);
		Corrade::Containers::ArrayView<const Ty> fullView{ vertices.data(), capacity };
		vertexPosBuffer.setData(fullView);
	}
	else {
		Corrade::Containers::ArrayView<const Ty> newestAdditions{&vertices[insertedIndex], sizeInserted};
		vertexPosBuffer.setSubData(insertedIndex * sizeof(Ty), newestAdditions);
	}
}
