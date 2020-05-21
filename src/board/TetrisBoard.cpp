#include "TetrisBoard.h"
#include "tetromino/Tetronimo.h"
#include <algorithm>
#include <type_traits>
#include <ciso646>
#include <optional>
#ifdef TETRIS_DEBUG_ASSERTIONS
#include <assert.h>
#endif

std::optional<tetris_clone::Tetronimo::Rotation> toTetronimoRotation(char dir) {
#ifdef TETRIS_DEBUG_ASSERTIONS
	assert(!(dir < 0) && dir < 4);
#endif
	using namespace tetris_clone;
	switch (dir) {
	case 0: return Tetronimo::Rotation::North;
		break;
	case 1: return Tetronimo::Rotation::East;
		break;
	case 2: return Tetronimo::Rotation::South;
		break;
	case 3: return Tetronimo::Rotation::West;
		break;
	default: return {};
	}
}

#ifndef DEBUG_ME
tetris_clone::TetrisBoard::TetrisBoard() noexcept : _firstRowFull{ -1 }, _clearedRows{ -1, -1, -1, -1 }, _clearedRowsSize{}{
#else
tetris_clone::TetrisBoard::TetrisBoard() noexcept : _firstRowFull{ -1 }, _clearedRows{ -1, -1, -1, -1 }, _clearedRowsSize{}, rowsFullSize_{}, _debugRep{}{
#endif

	_rowIsFull[(sizeof(_rowIsFull)/sizeof(bool))-1] = false;

	for (int i = 0; i < ROW_LENGTH; ++i) {
		for (int j = 0; j < COL_LENGTH; ++j) {
			_Board[i][j] = { TetrominoTile::Null, false };
		}
	}
}

bool tetris_clone::TetrisBoard::hasVacancy(unsigned int x, unsigned int y) const noexcept {
	int r = toRcCoordinatesY(y);
	int c = toRcCoordinatesX(x);

	if (_Board[r][c].type != TetrominoTile::Null)
		return false;
	else
		return true;

	//return false;
}

bool tetris_clone::TetrisBoard::getFullTetroPositions(Vector2c (&dst)[4], char x, char y) const {
	int r = toRcCoordinatesY(y);
	int c = toRcCoordinatesX(x);
	if (_Board[r][c].type == TetrominoTile::Null) {
		return false;
	}

	auto type = _Board[r][c].type;
	auto rotation = toTetronimoRotation(_Board[r][c].rotation);

	Vector2c results[4];

	if (rotation) {
		Tetronimo::tetroPositions(results, *rotation, type);
	}
	else {
		return false;
	}

	int posX = _Board[r][c].x;
	int posY = _Board[r][c].y;
	for (int i = 0; i < 4; ++i) {
		dst[i] = {posX + results[i].first, posY + results[i].second };
	}

	return true;
}
tetris_clone::BoardCell tetris_clone::TetrisBoard::getCellInfo(int x, int y) const {
	int r = toRcCoordinatesY(y);
	int c = toRcCoordinatesX(x);

	return _Board[r][c];
}

int tetris_clone::TetrisBoard::toRcCoordinatesX(int x) const noexcept {
	return x;
}
int tetris_clone::TetrisBoard::toRcCoordinatesY(int y) const noexcept {
	return (ROW_LENGTH - 1) - y;
}

int tetris_clone::TetrisBoard::toTetrisCoordinatesR(int r) const noexcept {
	return (ROW_LENGTH - 1) - r;
}

int tetris_clone::TetrisBoard::toTetrisCoordinatesC(int c) const noexcept {
	return c;
}

void tetris_clone::TetrisBoard::markRows() noexcept {

#if 1
	bool fullMarkedRows[ROW_LENGTH];

	for (int i = 0; i < ROW_LENGTH; ++i) {
		fullMarkedRows[i] = true;
		_rowIsFull[i] = true;
	}

	BoardCell (*rowPtr)[COL_LENGTH] = _Board;

	for (int i = 0; i < ROW_LENGTH; ++i) {
		for (int j = 0; j < COL_LENGTH; ++j) {
			if (rowPtr[i][j].type == TetrominoTile::Null) {
				fullMarkedRows[i] = false;
				_rowIsFull[i] = false;
				break;
			}
		}
		if (_rowIsFull[i]) {
			rowsFullSize_++;
			if (_firstRowFull < 0) {
				_firstRowFull = i;
			}
		}
	}
#endif
}

bool tetris_clone::TetrisBoard::isFull(int row) const noexcept { return _rowIsFull[row]; }


void tetris_clone::TetrisBoard::placeTetromino(int(&x)[4], int(&y)[4], const tetris_clone::Tetronimo& tetro) {
	for (int i = 0; i < 4; ++i) {
		_Board[toRcCoordinatesY(y[i])][toRcCoordinatesX(x[i])].type = tetro.toTile();
		_Board[toRcCoordinatesY(y[i])][toRcCoordinatesX(x[i])].slotIndex = i;
		char rot;
		switch (tetro.rotation()) {
		case Tetronimo::Rotation::North: rot = 0;
			break;
		case Tetronimo::Rotation::East: rot = 1;
			break;
		case Tetronimo::Rotation::South: rot = 2;
			break;
		case Tetronimo::Rotation::West: rot = 3;
			break;
		}
		_Board[toRcCoordinatesY(y[i])][toRcCoordinatesX(x[i])].rotation = rot;
		_Board[toRcCoordinatesY(y[i])][toRcCoordinatesX(x[i])].x = tetro.x();
		_Board[toRcCoordinatesY(y[i])][toRcCoordinatesX(x[i])].y = tetro.y();

	}
	markRows();
#ifdef DEBUG_ME
	updateString();
#endif
}

int tetris_clone::TetrisBoard::indexOfFullRow(int rowIndex) const noexcept {
	if (_firstRowFull < 0 or rowIndex > (COL_LENGTH-1) )
		return -1;
	if (_firstRowFull > rowIndex or _firstRowFull == rowIndex)
		return _firstRowFull;
	for (int i = _firstRowFull + 1; i < COL_LENGTH; ++i) {
		if (_rowIsFull[i])
			return i;
	}
	return -1;
}

tetris_clone::TetrisBoard::RowsClearedT tetris_clone::TetrisBoard::clearFullRows() noexcept {



	clearClearedRows();

	int fullRowsCountB{};
	int startingFullRowsIndexB{};
	int endingFullRowsIndexB{};

	for (int i = ROW_LENGTH - 1; !(i < 0); --i) {
		if (_rowIsFull[i]) {
			if ( !_rowIsFull[i+1] )
				endingFullRowsIndexB = i+1;
			fullRowsCountB += 1;
			startingFullRowsIndexB = i;
		}
		else if ( _rowIsFull[i+1] ) {
			insertClearedRow(i+1); // TODO: Useless?
			break;
		}
	}

	int fullRowsCountA{};
	int startingFullRowsIndexA{};
	int endingFullRowsIndexA{};

	if (fullRowsCountB) {
		for (int i = startingFullRowsIndexB - 2; !(i < 0); --i) {
			if (_rowIsFull[i]) {
				if (!_rowIsFull[i + 1])
					endingFullRowsIndexA = i + 1;
				fullRowsCountA += 1;
				startingFullRowsIndexA = i;
			}
			else if (_rowIsFull[i + 1]) {
				break;
			}
		}
	}

	if (fullRowsCountB != 0) {
		BoardCell* first = &_Board[0][0];
		BoardCell* last = &_Board[startingFullRowsIndexB][0];
		std::copy_backward(first, last, &_Board[endingFullRowsIndexB][0]);
		std::copy_backward(&_rowIsFull[0], &_rowIsFull[startingFullRowsIndexB], &_rowIsFull[endingFullRowsIndexB]);

		// if the earliest filled row recorded was the rows removed, then there was only one chunk of filled rows
		if (_firstRowFull == startingFullRowsIndexB)
			_firstRowFull = -1;
		else
			_firstRowFull += fullRowsCountB;
	}
	if (fullRowsCountA != 0 and fullRowsCountB != 0) {
		BoardCell* first = &_Board[0][0];
		BoardCell* last = &_Board[startingFullRowsIndexA + fullRowsCountB][0];
		std::copy_backward(first, last, &_Board[endingFullRowsIndexA + fullRowsCountB][0]);
		std::copy_backward(&_rowIsFull[0], &_rowIsFull[startingFullRowsIndexA + fullRowsCountB], &_rowIsFull[endingFullRowsIndexA+fullRowsCountB]);
	}

#ifdef DEBUG_ME
	updateString();
#endif
	auto narliestRow = toTetrisCoordinatesR(endingFullRowsIndexB-1);
	decltype(narliestRow) narliestRow2nd = -1;

	if (fullRowsCountA)
		narliestRow2nd = toTetrisCoordinatesR(endingFullRowsIndexA - 1);

	rowsFullSize_++;

	return RowsClearedT{static_cast<decltype(RowsClearedT::sizeFirst)>(fullRowsCountB), static_cast<decltype(RowsClearedT::rowIndexFirst)>(narliestRow),
	static_cast<decltype(RowsClearedT::sizeSecond)>(fullRowsCountA), static_cast<decltype(RowsClearedT::rowIndexSecond)>(narliestRow2nd)};
}

#ifdef DEBUG_ME
void tetris_clone::TetrisBoard::updateString() {

	if (_debugRep.empty()) {
		_debugRep = std::string((COL_LENGTH + 1) * ROW_LENGTH, ' ');
	}

	int index = 0;
	for (int i = 0; i < ROW_LENGTH; ++i) {
		for (int j = 0; j < COL_LENGTH; ++j) {
#if 0
			if (_Board[i][j].type == TetrominoTile::Null) {
				if ((i) % 5 == 4)
					_debugRep[index] = '_';
				else
					_debugRep[index] = ' ';
			}
			else {
#endif
				switch (_Board[i][j].type) {
				case TetrominoTile::I: _debugRep[index] = 'i';
					break;
				case TetrominoTile::J: _debugRep[index] = 'j';
					break;
				case TetrominoTile::L: _debugRep[index] = 'l';
					break;
				case TetrominoTile::T: _debugRep[index] = 't';
					break;
				case TetrominoTile::S: _debugRep[index] = 's';
					break;
				case TetrominoTile::Z: _debugRep[index] = 'z';
					break;
				case TetrominoTile::O: _debugRep[index] = 'o';
					break;
				default: if ((i) % 5 == 4)
					_debugRep[index] = '_';
					   else
					_debugRep[index] = '.';
					break;
				}
#if 0
				}
#endif
			index++;
		}
		_debugRep[index++] = '\n';
	}
}
#endif


int tetris_clone::TetrisBoard::operator[](int i) const noexcept {
	if (!(i < (sizeof(_clearedRows) / sizeof(std::remove_extent_t<decltype(_clearedRows)>))) or i < 0) {
		return -1;
	}
	return toTetrisCoordinatesR( _clearedRows[i]);
}

bool tetris_clone::TetrisBoard::insertClearedRow(int i) {
	if (_clearedRows[(sizeof(_clearedRows) / sizeof(std::remove_extent_t<decltype(_clearedRows)>)) - 2] != -1)
		return false;
	_clearedRows[_clearedRowsSize++] = i;
	return true;
}

void tetris_clone::TetrisBoard::clearClearedRows() {
	_clearedRowsSize = 0;
	int i = 0;
	while (_clearedRows[i] != -1)
		_clearedRows[i++] = -1;
}

#ifdef DEBUG_ME
std::string_view tetris_clone::TetrisBoard::getStr() const {
	int offset = 20 * 11;
	return std::string_view(_debugRep.c_str() + offset);
}
#endif
