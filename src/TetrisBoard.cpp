#include "TetrisBoard.h"
#include <algorithm>
#include <type_traits>
#include <ciso646>

#ifndef DEBUG_ME
tetris_clone::TetrisBoard::TetrisBoard() noexcept : _firstRowFull{ -1 }, _clearedRows{ -1, -1, -1, -1 }, _clearedRowsSize{}{
#else
tetris_clone::TetrisBoard::TetrisBoard() noexcept : _firstRowFull{ -1 }, _debugRep{}, _clearedRows{ -1, -1, -1, -1 }, _clearedRowsSize{} {
#endif

	constexpr int sizec = sizeof(_Board[0]) / sizeof(Cell);
	constexpr int sizer = (sizeof(_Board) / sizeof(Cell))/sizec;

	_rowIsFull[(sizeof(_rowIsFull)/sizeof(bool))-1] = false;

	for (int i = 0; i < sizer; ++i) {
		for (int j = 0; j < sizec; ++j) {
			_Board[i][j] = { TetrominoTile::I, false };
		}
	}
}

bool tetris_clone::TetrisBoard::hasVacancy(unsigned int x, unsigned int y) const noexcept {
	int r = toRcCoordinatesY(y);
	int c = toRcCoordinatesX(x);

	if (_Board[r][c].hasTile)
		return false;
	else
		return true;

	//return false;
}
bool tetris_clone::TetrisBoard::isMortalitized(unsigned int x, unsigned int y) const noexcept {
	int r = toRcCoordinatesY(y);
	int c = toRcCoordinatesX(x);

	if (_Board[r][c].mortalitized)
		return true;
	else
		return false;
}
int tetris_clone::TetrisBoard::toRcCoordinatesX(int x) const noexcept {
	return x;
}
int tetris_clone::TetrisBoard::toRcCoordinatesY(int y) const noexcept {

	constexpr int sizec = sizeof(_Board[0]) / sizeof(Cell);
	constexpr int sizer = (sizeof(_Board) / sizeof(Cell))/sizec;
	return (sizer - 1) - y;
}

int tetris_clone::TetrisBoard::toTetrisCoordinatesR(int r) const noexcept {
	constexpr int sizec = sizeof(_Board[0]) / sizeof(Cell);
	constexpr int sizer = (sizeof(_Board) / sizeof(Cell))/sizec;

	return (sizer - 1) - r;
}

int tetris_clone::TetrisBoard::toTetrisCoordinatesC(int c) const noexcept {
	return c;
}

void tetris_clone::TetrisBoard::markRows() noexcept {
	constexpr int sizec = sizeof(_Board[0]) / sizeof(Cell);
	constexpr int sizer = (sizeof(_Board) / sizeof(Cell))/sizec;

	bool fullMarkedRows[sizer];

	for (int i = 0; i < sizer; ++i) {
		fullMarkedRows[i] = true;
		_rowIsFull[i] = true;
	}

	Cell (*rowPtr)[sizec] = _Board;
	bool success{ true };
	for (int i = 0; i < sizer; ++i) {
		for (int j = 0; j < sizec; ++j) {
			if (!rowPtr[i][j].hasTile) {
				fullMarkedRows[i] = false;
				_rowIsFull[i] = false;
				break;
			}
		}
		if (_rowIsFull[i] and _firstRowFull < 0)
			_firstRowFull = i;
	}
}

bool tetris_clone::TetrisBoard::isFull(int row) const noexcept { return _rowIsFull[row]; }


void tetris_clone::TetrisBoard::placeTetromino(int(&x)[4], int(&y)[4], tetris_clone::TetrisBoard::TetrominoTile tileType) {
	for (int i = 0; i < 4; ++i) {
		_Board[toRcCoordinatesY(y[i])][toRcCoordinatesX(x[i])].hasTile = true;
		_Board[toRcCoordinatesY(y[i])][toRcCoordinatesX(x[i])].type = tileType;

	}
	markRows();
#ifdef DEBUG_ME
	updateString();
#endif
}

int tetris_clone::TetrisBoard::indexOfFullRow(int rowIndex) const noexcept {
	constexpr int sizec = sizeof(_Board[0]) / sizeof(Cell);
	if (_firstRowFull < 0 or rowIndex > (sizec-1) )
		return -1;
	if (_firstRowFull > rowIndex or _firstRowFull == rowIndex)
		return _firstRowFull;
	for (int i = _firstRowFull + 1; i < sizec; ++i) {
		if (_rowIsFull[i])
			return i;
	}
	return -1;
}

tetris_clone::TetrisBoard::RowsClearedT tetris_clone::TetrisBoard::clearFullRows() noexcept {
	constexpr int sizec = sizeof(_Board[0]) / sizeof(Cell);
	constexpr int sizer = (sizeof(_Board) / sizeof(Cell))/sizec;


	int higherStartingFullRowIndex{};
	int higherEndingRowIndex{};
	int fullRowsCount{};
//	int fullRowsCount2nd{};

	int fullRowsCount2nd{};
	int startingFullRowIndex2nd = -1;

	int lowerStartingFullRowIndex{};
	int lowerEndingRowIndex{};

	clearClearedRows();

	for (int i = sizer - 1; !(i < 0); --i) {
		if (_rowIsFull[i]) {
			if ( !_rowIsFull[i+1] )
				higherEndingRowIndex = i+1;
			fullRowsCount += 1;
			higherStartingFullRowIndex = i;
		}
		else if ( _rowIsFull[i+1] ) {//wasFullLastIteration) {
			insertClearedRow(i+1);
			break;
		}
	}

	if (fullRowsCount) {
		for (int i = higherStartingFullRowIndex - 2; !(i < 0); --i) {
			if (_rowIsFull[i]) {
				if (!_rowIsFull[i + 1])
					lowerEndingRowIndex = i + 1;
				fullRowsCount2nd += 1;
				lowerStartingFullRowIndex = i;
			}
			else if (_rowIsFull[i + 1]) {
				break;
			}
		}
	}

	if (fullRowsCount != 0) {
		Cell* first = &_Board[0][0];
		Cell* last = &_Board[higherStartingFullRowIndex][0];
		std::copy_backward(first, last, &_Board[higherEndingRowIndex][0]);//&_Board[sizer-1][sizec-1]+1);
		std::copy_backward(&_rowIsFull[0], &_rowIsFull[higherStartingFullRowIndex], &_rowIsFull[higherEndingRowIndex]);

		// if the earliest filled row recorded was the rows removed, then there wa only one chunk of filled rows
		if (_firstRowFull == higherStartingFullRowIndex)//earliestRow)
			_firstRowFull = -1;
		else
			_firstRowFull += fullRowsCount;
	}
	if (fullRowsCount2nd != 0 and fullRowsCount != 0) {
		Cell* first = &_Board[0][0];
		Cell* last = &_Board[lowerStartingFullRowIndex + fullRowsCount][0];
		std::copy_backward(first, last, &_Board[lowerEndingRowIndex + fullRowsCount][0]);
		std::copy_backward(&_rowIsFull[0], &_rowIsFull[lowerStartingFullRowIndex + fullRowsCount], &_rowIsFull[lowerEndingRowIndex+fullRowsCount]);
	}

#ifdef DEBUG_ME
	updateString();
#endif
	auto narliestRow = toTetrisCoordinatesR(higherEndingRowIndex-1);
	decltype(narliestRow) narliestRow2nd;

	if (fullRowsCount2nd)
		narliestRow2nd = toTetrisCoordinatesR(lowerEndingRowIndex - 1);

	return RowsClearedT{static_cast<decltype(RowsClearedT::sizeFirst)>(fullRowsCount), static_cast<decltype(RowsClearedT::rowIndexFirst)>(narliestRow),
	static_cast<decltype(RowsClearedT::sizeSecond)>(fullRowsCount2nd), static_cast<decltype(RowsClearedT::rowIndexSecond)>(narliestRow2nd)};
}

#ifdef DEBUG_ME
void tetris_clone::TetrisBoard::updateString() {
	constexpr int sizec = sizeof(_Board[0]) / sizeof(Cell);
	constexpr int sizer = (sizeof(_Board) / sizeof(Cell))/sizec;

	if (_debugRep.empty()) {
		_debugRep = std::string((sizec + 1) * sizer, ' ');
	}

	int index = 0;
	for (int i = 0; i < sizer; ++i) {
		for (int j = 0; j < sizec; ++j) {
			if (!_Board[i][j].hasTile) {
				if ((i) % 5 == 4)
					_debugRep[index] = '_';
				else
					_debugRep[index] = ' ';
			}
			else {
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
				}
			}
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
