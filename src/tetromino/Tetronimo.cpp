#include "Tetronimo.h"
#include <tuple>
#include <limits>
#include <random>
#include <ciso646>



static std::mt19937 rng_engine{ 137 };
static std::uniform_int_distribution<unsigned int> dist{ 0, 6 };

const static tetris_clone::Vector2i S_ROTATIONS[4][4] =
{
	{{0, 1}, {1, 1}, {1, 2}, {2, 2}},
{{ 1, 2}, {1, 1}, {2, 1}, {2, 0} },
{{2, 1}, {1, 1}, {1, 0}, {0, 0}},
{{1, 0}, {1, 1}, {0, 1}, {0, 2}}
};

const static tetris_clone::Vector2i Z_ROTATIONS[4][4] =
{
	{{0,2}, {1, 2}, {1, 1}, {2, 1}},
{{2, 2}, {2, 1}, {1, 1}, {1, 0}},
{{2, 0}, {1, 0}, {1, 1}, {0, 1}},
{{0, 0}, {0, 1}, {1, 1}, {1, 2}}
};

const static tetris_clone::Vector2i T_ROTATIONS[4][4] =
{
	{{1, 2}, {0, 1}, {1, 1},{2, 1}},
{{2, 1}, {1, 2}, {1, 1}, {1, 0}},
{{1, 0}, {2, 1}, {1, 1}, {0, 1}},
{{0, 1}, {1, 0}, {1, 1}, {1, 2}}
};

const static tetris_clone::Vector2i J_ROTATIONS[4][4] =
{
	{{0, 1}, {1, 1}, {2, 1}, {2, 2}},
{{1, 2}, {1, 1}, {1, 0}, {2, 0}},
{{2, 1}, {1, 1}, {0, 1}, {0, 0}},
{{1, 0}, {1, 1}, {1, 2}, {0, 2}}
};

#if 0
const static tetris_clone::Vector2i L_ROTATIONS[4][4] =
{
	{{0, 2}, {0, 1}, {1, 1}, {2, 1}},
{{2, 2}, {1, 2}, {1, 1}, {1, 0}},
{{2, 1}, {1, 1}, {0, 1}, {0, 0} },
{{1, 2}, {1, 1}, {1, 0}, {0, 0} }
};
#else
const static tetris_clone::Vector2i L_ROTATIONS[4][4] =
{
	{{0, 2}, {0, 1}, {1, 1}, {2, 1}},
{{2, 2}, {1, 2}, {1, 1}, {1, 0}},
{{2, 0}, {2, 1}, {1, 1}, {0, 1} },
{{0, 0}, {1, 0}, {1, 1}, {1, 2} }
};

#endif

const static tetris_clone::Vector2i I_ROTATIONS[4][4] =
{ {{1, 2},{2, 2},{3,2},{4,2}},
{{2, 3}, {2, 2}, {2, 1}, {2, 0}},
{{3, 2}, {2, 2}, {1, 2}, {0,2}},
{{2, 1}, {2, 2}, {2, 3}, {2, 4}}
};

const static tetris_clone::Vector2i O_ROTATIONS[4][4] =
{
{ {1,2}, {2, 2}, {2, 1}, {1, 1} },
{ {2, 1}, {2, 0}, {1, 0}, {1, 1} },
{ {1, 0}, {0, 0}, {0, 1}, {1, 1} },
{ {0, 1}, {0, 2}, {1, 2}, {1, 1} }
};


#if 1
tetris_clone::Tetronimo::Tetronimo(Flavor f) : _position{ 0, 0 } {
	replaceFlavorWith(f);
}
#endif

tetris_clone::Tetronimo::Tetronimo(Flavor f, std::pair<unsigned int, unsigned int> pos) : _position{ pos } {
	replaceFlavorWith(f);
}

tetris_clone::Tetronimo& tetris_clone::Tetronimo::operator--() noexcept {

#if 0

	char val = (static_cast<char>(_direction)+1)%4;
	switch (val) {
	case 0: _direction = Rotation::North;
		_rotationState -= 3;
		break;
	case 1: _direction = Rotation::East;
		++_rotationState;
		break;
	case 2: _direction = Rotation::South;
		++_rotationState;
		break;
	case 3: _direction = Rotation::West;
		++_rotationState;
		break;
	}
#endif
	switch (_direction) {
	case Rotation::West: _direction = Rotation::North;
		_rotationState -= 3;
		break;
	case Rotation::North: _direction = Rotation::East;
		++_rotationState;
		break;
	case Rotation::East: _direction = Rotation::South;
		++_rotationState;
		break;
	case Rotation::South: _direction = Rotation::West;
		++_rotationState;
		break;
	}

	return *this;
}

tetris_clone::Tetronimo& tetris_clone::Tetronimo::operator++() noexcept {

#if 0

	char val = ((static_cast<char>(_direction)-1)+4)%4;
	switch (val) {
	case 0: _direction = Rotation::North;
		--_rotationState;
		break;
	case 1: _direction = Rotation::East;
		--_rotationState;
		break;
	case 2: _direction = Rotation::South;
		--_rotationState;
		break;
	case 3: _direction = Rotation::East;
		_rotationState += 3;
		break;
	}
#else
	switch (_direction) {
	case Rotation::East: _direction = Rotation::North;
		--_rotationState;
		break;
	case Rotation::South: _direction = Rotation::East;
		--_rotationState;
		break;
	case Rotation::West: _direction = Rotation::South;
		--_rotationState;
		break;
	case Rotation::North: _direction = Rotation::West;
		_rotationState += 3;
		break;
	}
#endif

	return *this;
}

void tetris_clone::Tetronimo::setRotation(const tetris_clone::Tetronimo::Rotation r) noexcept {
	int i, j, k;
	switch (r) {
	case Rotation::East: --* this;
		break;
	case Rotation::South: --* this;
		--* this;
		break;
	case Rotation::West: --* this;
		--* this;
		--* this;
		break;
	case Rotation::North: i = 1;
		j = i;
		k = j;
		break;
	default:
		break;
	}
	return;
}
void tetris_clone::Tetronimo::replaceFlavorWith(Flavor f)
{
	_classification = f;
	_direction = Rotation::North;

	switch (_classification) {
	case Flavor::S:
		_rotationState = &S_ROTATIONS[0];
		_aabb = 3;
		break;
	case Flavor::Z:
		_rotationState = &Z_ROTATIONS[0];
		_aabb = 3;
		break;
	case Flavor::T:
		_rotationState = &T_ROTATIONS[0];
		_aabb = 3;
		break;
	case Flavor::J:
		_rotationState = &J_ROTATIONS[0];
		_aabb = 3;
		break;
	case Flavor::L:
		_rotationState = &L_ROTATIONS[0];
		_aabb = 3;
		break;
	case Flavor::I:
		_rotationState = &I_ROTATIONS[0];
		_aabb = 5;
		break;
	case Flavor::O:
		_rotationState = &O_ROTATIONS[0];
		_aabb = 3;
		break;
	}
}

int tetris_clone::Tetronimo::x() const noexcept { return _position.first; }
int tetris_clone::Tetronimo::y() const noexcept { return _position.second; }

unsigned int tetris_clone::Tetronimo::moveNBlocksForwardX(int n) noexcept {
#if 0
	if (n < 0 && (_position.first + n) >(std::numeric_limits<unsigned int>::max() + n))
		std::exit(-3);
	if (n > 0 && (_position.first + n) > std::numeric_limits<unsigned int>::max())
		std::exit(-4);
#endif

	_position.first += n;

	unsigned int res;
	if (n < 0)
		res = -n;
	else
		res = n;
	return res;
}

unsigned int tetris_clone::Tetronimo::moveNBlocksForwardY(int n) noexcept {
#if 0
	if (n < 0 && (_position.second + n) >(std::numeric_limits<unsigned int>::max() + n))
		std::exit(-3);
	if (n > 0 && (_position.second + n) > std::numeric_limits<unsigned int>::max())
		std::exit(-4);
#endif

	_position.second += n;

	unsigned int res;
	if (n < 0)
		res = -n;
	else
		res = n;
	return res;
}

void tetris_clone::Tetronimo::genTetronimos(tetris_clone::Tetronimo::Flavor(&src)[7]) noexcept {
	struct helper_t {
		bool taken;
		Flavor flavor;
	};

	helper_t slots[7] = { {false, Flavor::I}, {false, Flavor::J}, {false, Flavor::L}, {false, Flavor::O}, {false, Flavor::S}, {false, Flavor::T}, {false, Flavor::Z} };

	int n = 0;

	int i = 0;
	while (n < 7) {
		auto index = dist(rng_engine);
		if (!slots[index].taken) {
			src[i++] = slots[index].flavor;
			slots[index].taken = true;
			n++;
		}
	}
	return;
}
bool tetris_clone::Tetronimo::canRegressY(const tetris_clone::TetrisBoard& board) {
	if (_position.second < 0) {
		if (_classification != Flavor::I and _direction == Rotation::North) {
			return false;
		}
		if ((_classification == Flavor::O or _classification == Flavor::I) and _direction == Rotation::West) {
			return false;
		}
		// This has to be I-flavor
		// The branches accounts for two orientations of O-tetronimo
		if (_position.second == -2)
			return false;

	}
	auto testPosition = Vector2i{ _position.first, _position.second - 1 };

	if (testPosition.second < 0) {
		if (_classification == Flavor::O and (_direction==Rotation::West or _direction == Rotation::North)) {}
		else if (_classification != Flavor::I and _direction != Rotation::North)
			return false;
		if(_classification == Flavor::I and _direction == Rotation::East)
			return false;
	}

	for (int i = 0; i < 4; ++i) {
		if (!board.hasVacancy(testPosition.first + _rotationState[0][i].first, testPosition.second + _rotationState[0][i].second))  // Watch this during debugging! Illegal pointer access bug might be present
			return false;
	}
	return true;

}

bool tetris_clone::Tetronimo::canRegressX(const tetris_clone::TetrisBoard& board) {
	if (_position.first < 0) {
		if (_classification != Flavor::I and _direction == Rotation::East) {
			return false;
		}
		if ((_classification == Flavor::O or _classification == Flavor::I) and _direction == Rotation::North) {
			return false;
		}
		if (_position.first == -2)
			return false;
		// This has to be I-flavor
		// This branch accounts for two orientations of O-tetronimo
	}
	auto testPosition = Vector2i{ _position.first - 1, _position.second };

	if (testPosition.first < 0) {
		if (_classification == Flavor::O and (_direction == Rotation::North or _direction == Rotation::East)) {}
		else if (_classification != Flavor::I and _direction != Rotation::East)
			return false;
		if(_classification == Flavor::I and _direction == Rotation::South)
			return false;
	}

	for (int i = 0; i < 4; ++i) {
		if (!board.hasVacancy(testPosition.first + _rotationState[0][i].first, testPosition.second + _rotationState[0][i].second))  // Watch this during debugging! Illegal pointer access bug could happen!
			return false;
	}
	return true;

}

bool tetris_clone::Tetronimo::canAdvanceX(const tetris_clone::TetrisBoard& board) {
	if ((static_cast<int64_t>(_position.first) + _aabb) > board.rowLengths()) {
		if (_classification != Flavor::I && _direction == Rotation::West) {
			return false;
		}
		if ((_classification == Flavor::O || _classification == Flavor::I) && _direction == Rotation::South) {
			return false;
		}
		if (_position.first == ((board.rowLengths() - _aabb) + 2))
			return false;
		//if( _classification == Flavor::I && _direction == Rotation::South)
		// This has to be I flavor
		// This branch accounts for two orientations of O-tetronimo
	}
	auto testPosition = Vector2i{ _position.first + 1, _position.second };

	if ((static_cast<int64_t>(testPosition.first) + _aabb) > board.rowLengths()) {
		if(_classification==Flavor::O and (_direction==Rotation::South or _direction==Rotation::West)){ }
		else if (_classification != Flavor::I and _direction != Rotation::West)
			return false;
		if (_classification == Flavor::I && _direction == Rotation::North)
			return false;
	}

	// now test for collision against placed tetronimos
	for (int i = 0; i < 4; ++i) {
		if (!board.hasVacancy(testPosition.first + _rotationState[0][i].first, testPosition.second + _rotationState[0][i].second)) // WATCH THIS! Illegal pointer access bug could be possible!
			return false;
	}
	return true;
}

tetris_clone::Vector2i tetris_clone::Tetronimo::getRotatedPositionOf(const char slot) const noexcept {
	return _rotationState[0][static_cast<std::underlying_type_t<RotatedTetroSlot>>(slot)];
}

tetris_clone::Tetronimo::Flavor tetris_clone::Tetronimo::simpleSpawn() {
	auto flavor = dist(rng_engine);
	switch (flavor) {
	case 0: return Flavor::S;
	case 1: return Flavor::Z;
	case 2: return Flavor::T;
	case 3: return Flavor::J;
	case 4: return Flavor::L;
	case 5: return Flavor::I;
	case 6: return Flavor::O;
	default: std::exit(-33);
	}
}

tetris_clone::TetrisBoard::TetrominoTile tetris_clone::Tetronimo::toTile() const noexcept {
	switch (_classification) {
	case Flavor::S: return TetrisBoard::TetrominoTile::S;
	case Flavor::Z: return TetrisBoard::TetrominoTile::Z;
	case Flavor::T: return TetrisBoard::TetrominoTile::T;
	case Flavor::J: return TetrisBoard::TetrominoTile::J;
	case Flavor::L: return TetrisBoard::TetrominoTile::L;
	case Flavor::I: return TetrisBoard::TetrominoTile::I;
	case Flavor::O: return TetrisBoard::TetrominoTile::O;
	default: exit(-34);
	}
}