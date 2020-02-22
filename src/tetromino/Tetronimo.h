#pragma once
#ifndef __TETRONIMO_H__
#define __TETRONIMO_H__
#include <tuple>
#include <cstdint>

#include "board/TetrisBoard.h"

namespace tetris_clone {
	using Vector2ui = std::pair<unsigned int, unsigned int>;
	using Vector2i = std::pair<int, int>;

	//class TetrisBoard;

	class Tetronimo {
	public:
		enum class Rotation : char {
			North = 1, East = 0, South = 3, West = 2
		};
		enum class Flavor : uint32_t {
			S = 0x96f905ff,
			Z = 0xe10808ff,
			T = 0xdd01e0ff,
			J = 0x5a5a97ff,
			L = 0xfb5714ff,
			I = 0x00ecffff,
			O = 0xfbd01eff
		};
		enum class RotatedTetroSlot : char {
			first = 0, second = 1, third = 2, last = 3
		};
		Tetronimo(Flavor f);
		Tetronimo(Flavor f, std::pair<unsigned int, unsigned int> pos);
		void replaceFlavorWith(Flavor f);

		Tetronimo& operator++() noexcept;
		Tetronimo& operator--() noexcept;
		void setRotation(const Rotation r) noexcept;

		int x() const noexcept;
		int y() const noexcept;
		unsigned int moveNBlocksForwardX(int n) noexcept;
		unsigned int moveNBlocksForwardY(int n) noexcept;

		void genTetronimos(Flavor (&src)[7]) noexcept;

		static Flavor simpleSpawn();

		bool canRegressY(const TetrisBoard& board);
		bool canRegressX(const TetrisBoard& board);
		bool canAdvanceX(const TetrisBoard& board);
		Flavor flavor() const { return _classification; }
		Rotation rotation() const { return _direction; }
		Vector2i getRotatedPositionOf(const char slot) const noexcept;

		tetris_clone::TetrisBoard::TetrominoTile toTile() const noexcept;

	private:
		//static Flavor extractTetronimo(Flavor* src, size_t index, size_t size);
		Vector2i _position;
		uint16_t _aabb;
		const Vector2i (*_rotationState)[4];
		Flavor _classification;

		Rotation _direction;
	};
}

#endif
