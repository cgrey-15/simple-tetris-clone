#pragma once
#ifndef __TETRIS_BOARD_H__
#define __TETRIS_BOARD_H__

#include <tuple>
//#include "Tetronimo.h"

#define DEBUG_ME
#ifdef DEBUG_ME
#include <string>
#endif

namespace tetris_clone {
	class TetrisBoard {
	public:
		enum class TetrominoTile : char {
			I, O, L, S, Z, T, J
		};

		struct Cell {
			TetrominoTile type;
			bool hasTile{ false };
			bool mortalitized{ false };
		};


		TetrisBoard() noexcept;
		typedef Cell(*pointerToArrays)[10];
		pointerToArrays getBoard() { return _Board; }
		size_t rowCount() const { return sizeof(_Board[0]) / sizeof(Cell); }
		size_t rowLengths() const { return sizeof(_Board[0]) / sizeof(Cell); }
		bool hasVacancy(unsigned int x, unsigned int y) const noexcept;
		bool isMortalitized(unsigned int x, unsigned int y) const noexcept;
		bool isFull(int row)const noexcept;
		int indexOfFullRow(int rowIndex) const noexcept;
		void placeTetromino(int(&x)[4], int(&y)[4], TetrominoTile tileType);

		int operator[](int i) const noexcept;
		int clearedRowsLength() const noexcept { return _clearedRowsSize; }


		/**
		Postcondition: if there is at least one chunk (one or more rows) of filled rows,
		 then the highest chunk will be removed and everything before it
		 will be moved up by that-chunk-size rows.
		*/
		// If a row or more is cleared, there can be at most two non-consecutive rows
		// that are cleared
		struct RowsClearedT {
			char sizeFirst{ 0 };
			char rowIndexFirst;
			char sizeSecond{ 0 };
			char rowIndexSecond;
		};
		RowsClearedT clearFullRows() noexcept; // TODO: redo this so it can do two disjoint rows


	private:

		void markRows() noexcept;
		int toRcCoordinatesX(int x) const noexcept;
		int toRcCoordinatesY(int y) const noexcept;
		int toTetrisCoordinatesR(int r) const noexcept;
		int toTetrisCoordinatesC(int c) const noexcept;
#ifdef DEBUG_ME
		void updateString();
#endif
		bool insertClearedRow(const int i);
		void clearClearedRows();

		Cell _Board[40][10];
		bool _rowIsFull[41];	
		int _clearedRows[5];
		int _clearedRowsSize;
		int _firstRowFull;
#ifdef DEBUG_ME
		std::string _debugRep;
#endif
	};

}
#endif
