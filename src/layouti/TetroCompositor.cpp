#include "layouti/TetroCompositor.h"
#include "drawobjey/Sprite.h"
#include "board/TetrisBoard.h"

int subdivideTetros(layouti::TetroCompositor::Object2D& src, int n, layouti::TetroCompositor::Object2D& dst, int, int);
bool tetrosAboveRow(const layouti::TetroCompositor::Object2D*, int, int);

void layouti::TetroCompositor::processMe(char size, char lowestEmptyRowIndex, tetris_clone::TetrisBoard& board) {
#if 0
	using namespace drawobjey;
	using TetroOb2d = tetris_clone::TetronimoRendering::TetrominoObject2D;

	for (int i = lowestEmptyRowIndex; i < lowestEmptyRowIndex + size; ++i) {

		for (int j = 0; j < COLUMN_SIZE; ++j) {
			int tetroFirst = j;
			int k = tetroFirst;
			const Object2D* sameParent = tetros_[i][k]->parent();
			int tetroSize = 0;
			while (k < COLUMN_SIZE and tetros_[i][k]->parent() == sameParent) {
				int l = i;
				while (tetros_[l][k]->parent() == sameParent) {
					if (l > lowestEmptyRowIndex + size) {
						auto node = treeHandler
					}
					++l;
				}
				treeHandler_.makeDropping(tetros_[i][k]);
				k++;
			}
			j = k;
			Object2D* children[4] = { {}, {}, {}, {} };
		}
	}

	Object2D* _bottomBlob{};
	Object2D* _droppableBlob{};
	Object2D* _renderTilePool{};
	int startingRow;
	int _i;

	for (auto& tetrominoRenderChild : _bottomBlob->children()) {
		Object2D* fosterChildren[4] = { {}, {}, {}, {} };
		int fosterChildrenSize{};
		//packCoordsTogether(tetrominoRenderChild.children().first());
		for (auto* tileChild = dynamic_cast<TetroOb2d*>(tetrominoRenderChild.children().first()); tileChild != nullptr; tileChild = dynamic_cast<TetroOb2d*>(tileChild->nextSibling())) {
			for (int i = 0; i < size; ++i) {
				if (startingRow+i == tileChild->globalY) {
					fosterChildren[fosterChildrenSize++] = tileChild;
				}
			}
		}
		//packCoordsTogether(tetrominoRenderChild.children().first());

		for (int i = 0; i < fosterChildrenSize; ++i) {
			dynamic_cast<RudimentarySprite*>(fosterChildren[i]->features().first())->setVisibility(false);
			fosterChildren[i]->setParent(_renderTilePool);
		}
		//packCoordsTogether(tetrominoRenderChild.children().first());

		if (fosterChildrenSize > 0 and tetrominoRenderChild.children().first()) {
			if (fosterChildrenSize == 4) {} // This should not work; otherwise, logical fault present
			else
				auto silly = subdivideTetros(*tetrominoRenderChild.children().first(), 4 - fosterChildrenSize, *_droppableBlob, size, startingRow);
		}
	}
	if (!_bottomBlob->children().isEmpty()) {
		auto* childi = _bottomBlob->children().first();
		//packCoordsTogether(childi->children().first());
		while (childi and childi->nextSibling()) {
			//packCoordsTogether(childi->nextSibling()->children().first());
			while (childi->nextSibling() and tetrosAboveRow(childi->nextSibling()->children().first(), size, startingRow))
				childi->nextSibling()->setParent(_droppableBlob);
			childi = childi->nextSibling();
		}
		if (tetrosAboveRow(_bottomBlob->children().first()->children().first(), size, startingRow)) 
			_bottomBlob->children().first()->setParent(_droppableBlob);
	}
	//_placementMode = false;
	//_moveBlobMode = true;

#if 0 // this one!
	if (!_testMachine.is<RegState::dropping>()) {
		_testMachine.toggle<RegState::placement_mode>();
		_testMachine.toggle<RegState::move_blob_mode_start>();
		_testMachine.toggle<RegState::move_blob_mode>();
	}
#elif 0
	_testMachine.toggle<RegState::dropping>();
	_testMachine.toggle<RegState::beginning_drop>();
#else
#endif
	++_i;
#endif

	for (int ithRow = 0; ithRow < ROW_SIZE; ++ithRow) {
		Object2D* sameParent = nullptr;
		for (int jthCol = 0; jthCol < COLUMN_SIZE; ++jthCol) {
			if (sameParent == tetros_[ithRow][jthCol]->parent()) {
				jthCol++;
				continue;
			}
			tetris_clone::Vector2c otherPos[4];
			if (board.getFullTetroPositions(otherPos, jthCol, ithRow)) {

				tetris_clone::Vector2c higher[4] = { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} };
				tetris_clone::Vector2c lower[4] = { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} };
				int hiSize{};
				int loSize{};

				for (auto position: otherPos) {
					if (position.second > lowestEmptyRowIndex + size) {
						higher[hiSize++] = position;
					}
					else if (position.second < lowestEmptyRowIndex) {
						lower[loSize++] = position;
					}
				}
			}
			sameParent = tetros_[ithRow][jthCol];
		}
	}

	return;
}

void layouti::TetroCompositor::placeTetro() {
#if 0
	auto nodeHandles = treeHandler_.redeemAccess();

	nodeHandles.spawnNonmovingNode();
#endif
	auto [x, y] = tetrominoModel_.getRotatedPositionOf(0);

	//auto dstNodeHandle = treeHandler_.redeemAccess(tetros_[x][y]->parent());
	treeHandler_.makeNonmoving(tetros_[x][y]);

	return;
	
	// TODO: transfer entire node and not just tiles (the parent node too)
}

void layouti::TetroCompositor::react() {

}

void layouti::TetroCompositor::left(tetris_clone::TetrisBoard& b) {
	if (tetrominoModel_.canRegressX(b)) {
		tetrominoModel_.moveNBlocksForwardX(-1);
	}
}
void layouti::TetroCompositor::right(tetris_clone::TetrisBoard& b) {
	if (tetrominoModel_.canAdvanceX(b)) {
		tetrominoModel_.moveNBlocksForwardX(1);
	}
}
void layouti::TetroCompositor::up(tetris_clone::TetrisBoard& b) {
	tetrominoModel_.moveNBlocksForwardY(1);
}
void layouti::TetroCompositor::down(tetris_clone::TetrisBoard& b) {
	if (tetrominoModel_.canRegressY(b)) {
		tetrominoModel_.moveNBlocksForwardY(-1);
	}
} 