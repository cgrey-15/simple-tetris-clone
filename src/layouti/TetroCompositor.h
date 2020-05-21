#ifndef TETRO_COMPOSITOR_H
#define TETRO_COMPOSITOR_H

#include <Magnum/Scenegraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/Platform/Sdl2Application.h>
#include "layouti/NodeArrangementUnit.h"
#include "tetromino/Tetronimo.h"
//#include "drawobjey/Sprite.h"
#include "TetronimoRendering.h"
#include "support/PulseTimer.h"

namespace tetris_clone {
	class TetrisBoard;
	struct BoardCell;
}

namespace layouti {
	class TetroCompositor : public supp::PulseTimer::Pulsee {
		static constexpr int8_t ROW_SIZE = 40;
		static constexpr int8_t COLUMN_SIZE = 10;

	public:
		typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D> Object2D;

		TetroCompositor(Object2D& tetrisActorRoot);
		void processMe(char size, char lowestEmptyIndex, tetris_clone::TetrisBoard& board);
		void react() override;
		void updateScene();
		void placeTetro();

		void rotC() { ++tetrominoModel_; }
		void rotCC() { --tetrominoModel_; }
		void left(tetris_clone::TetrisBoard& b);
		void right(tetris_clone::TetrisBoard& b);
		void up(tetris_clone::TetrisBoard& b);
		void down(tetris_clone::TetrisBoard& b);

#if 0
		enum class State : int8_t {
			obseleted = 0,
			dropping,
			dropping_start,
			placement_mode,
			placement_mode_start,
			check_integri_mode,
			check_integri_mode_start,
			END
		};
		enum class MacroState : int8_t {
		};
#endif // state enum prototype design

	private:

		Object2D*                   tetros_[ROW_SIZE][COLUMN_SIZE];
		NodeArrangementUnit                          treeHandler_;

		tetris_clone::TetronimoRendering placementDrawTetroOwner_;
		tetris_clone::Tetronimo                   tetrominoModel_;


		bool freeChooseMode;
	};
}
#endif // TETRO_COMPOSITOR_H
