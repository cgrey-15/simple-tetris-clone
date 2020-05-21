#ifndef NODE_ARRANGEMENT_UNIT_H
#define NODE_ARRANGEMENT_UNIT_H

#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include "support/Utilities.hpp"
#include "support/Common.h"

namespace drawobjey {
	class RudimentarySprite;
	class SpriteHandler;
}

namespace Magnum {
	namespace Shaders {
		template<Magnum::UnsignedInt dimension>
		class Flat;
		
		typedef Flat<2> Flat2D;
	}
}

namespace layouti {
	/*
	* SUMMARY
	* Transfers globs of tetris draw-tiles. Handles either the keeper of
	tile-chunks or can also transfer tiles themselves.
	*/

	class NodeArrangementUnit {
		static constexpr int CARRY_CAPACITY = 5;
		typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D> Object2D;
	public:
		class Access {
			friend class NodeArrangementUnit;
			Access(NodeArrangementUnit& unit);
			Access(NodeArrangementUnit& unit, Object2D* tile);
		public:
			using NodePos = int;
			NodePos spawnNonmovingNode();
			NodePos spawnDroppingNode();
			NodePos openNode(Object2D* tile);

			NodePos tailPos() const;

			NodePos spawnTile(NodePos node);

			void transferTile(NodePos tilePassenger, Access& destimation);

			void addSprite(NodePos node, resource::RsrcId, drawobjey::SpriteHandler& handler, Magnum::Timeline& timeline, Magnum::SceneGraph::DrawableGroup2D& group, Magnum::Shaders::Flat2D& shader);
			bool hasSprite(NodePos node) const;
		private:
			NodeArrangementUnit& manipulant_;
			int objSize_;
			int sprSize_;
			Object2D*                     obj_[CARRY_CAPACITY];
			drawobjey::RudimentarySprite* spr_[CARRY_CAPACITY];
		};
		NodeArrangementUnit(Object2D& overarchingParent);
		Access redeemAccess();
		Access redeemAccess(Object2D* node);
		void makeDropping(Object2D* node);
		void makeNonmoving(Object2D* tile);
	private:

		enum class node_groups : uint8_t {
			nonmovingProps,
			droppingProps,
			COUNT
		};
		using NodeGroupTuple = supp::BasicTaggedTuple<Object2D*, node_groups::COUNT>;

		NodeGroupTuple nodeGroups_;
		Object2D placementNode_;
		Object2D& parent;
	};
}
#endif // NODE_ARRANGEMENT_UNIT_H
