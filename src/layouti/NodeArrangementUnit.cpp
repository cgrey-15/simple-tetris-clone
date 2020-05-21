#include <Magnum/Shaders/Flat.h>
#include "layouti/NodeArrangementUnit.h"
#include "drawobjey/SpriteHandler.h"
#ifdef TETRIS_DEBUG_ASSERTIONS
#include <cassert>
#endif

layouti::NodeArrangementUnit::Access::Access(NodeArrangementUnit& unit) : manipulant_{ unit }, objSize_{}, sprSize_{} {
}
layouti::NodeArrangementUnit::Access::Access(NodeArrangementUnit& unit, Object2D* tile) : 
	manipulant_{unit}, 
	objSize_{}, 
	sprSize_{} 
{
	obj_[objSize_++] = tile->parent();
	for(auto& tile : obj_[0]->children() ) {
		obj_[objSize_++] = &tile;
	}
}

layouti::NodeArrangementUnit::Access layouti::NodeArrangementUnit::redeemAccess() {
	return Access{ *this };
}

layouti::NodeArrangementUnit::Access layouti::NodeArrangementUnit::redeemAccess( Object2D* tile ) {
	return Access{ *this, tile };
}

void layouti::NodeArrangementUnit::makeNonmoving(Object2D* tile) {
	auto& parent = *tile->parent();
#ifdef TETRIS_DEBUG_ASSERTIONS
	assert(tile->children().isEmpty());
#endif
	parent.setParent(nodeGroups_.is<node_groups::nonmovingProps>());
}

using NodePosT = layouti::NodeArrangementUnit::Access::NodePos;

NodePosT layouti::NodeArrangementUnit::Access::spawnNonmovingNode() {
	int newHandler = objSize_++;
	if (!(objSize_ < CARRY_CAPACITY)) {
		std::exit(44);
	}
	obj_[newHandler] = new Object2D{manipulant_.nodeGroups_.is<node_groups::nonmovingProps>()};
	return newHandler;
}

NodePosT layouti::NodeArrangementUnit::Access::spawnDroppingNode() {
	int newHandler = objSize_++;
	obj_[newHandler] = new Object2D{manipulant_.nodeGroups_.is<node_groups::nonmovingProps>()};
	return newHandler;
}

NodePosT layouti::NodeArrangementUnit::Access::spawnTile(NodePos parent) {
	int newNode = objSize_++;
	if (!(objSize_ < CARRY_CAPACITY)) {
		std::exit(44);
	}
	obj_[newNode] = new Object2D{ obj_[parent] };
}

void layouti::NodeArrangementUnit::Access::transferTile(NodePos tilePassenger, Access& destination) {
	Object2D* transferNode = obj_[tilePassenger];
	for(int i = tilePassenger; i < objSize_-1; ++i) {
		obj_[i] = obj_[i+1];
	}
	objSize_--;

	destination.obj_[destination.objSize_++] = transferNode;
	transferNode->setParent(destination.obj_[0]);
	destination.sprSize_++;

}
void layouti::NodeArrangementUnit::Access::addSprite(Access::NodePos index, resource::RsrcId assetId, drawobjey::SpriteHandler& spriteSrv, Magnum::Timeline& timeline, Magnum::SceneGraph::DrawableGroup2D& group, Magnum::Shaders::Flat2D& shaders) {
	using namespace drawobjey;

#ifdef TETRIS_DEBUG_ASSERTIONS
	assert(index != 0);
#endif

	auto [width, height] = spriteSrv.getDimensions(assetId);
	auto& texture = spriteSrv.getTexture(assetId);
	auto vertBuffer = spriteSrv.getVertexBuffer(assetId);
	auto idxBuffer = spriteSrv.getIndicesBuffer(assetId);
	auto texCoordData = spriteSrv.getTexCoordData(assetId);
	RudimentarySprite* sprite = new RudimentarySprite(*obj_[index], width, height, 0.125f, texture, vertBuffer.offset, idxBuffer.offset, *vertBuffer.buffer, texCoordData, *idxBuffer.buffer, timeline, group, shaders);

	//obj_[index]
}

NodePosT layouti::NodeArrangementUnit::Access::tailPos() const {
	return objSize_;
}



