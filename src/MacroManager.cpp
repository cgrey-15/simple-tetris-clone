#include "MacroManager.h"
#if 0
tetris_clone::Tetronimo::Flavor toTetro(const char c) {
	tetris_clone::Tetronimo::Flavor result{};
	switch (c) {
		using namespace tetris_clone;
	case 'S': result = Tetronimo::Flavor::S;
		break;
	case 'Z': result = Tetronimo::Flavor::Z;
		break;
	case 'T': result = Tetronimo::Flavor::T;
		break;
	case 'J': result = Tetronimo::Flavor::J;
		break;
	case 'L': result = Tetronimo::Flavor::L;
		break;
	case 'I': result = Tetronimo::Flavor::I;
		break;
	case 'O': result = Tetronimo::Flavor::O;
		break;
	default: result = Tetronimo::Flavor::I;
	}
	return result;
}
tetris_clone::Tetronimo::Rotation toTetroRot(const char c) {
	tetris_clone::Tetronimo::Rotation rot{};

	switch (c) {
		using namespace tetris_clone;
	case 'n': rot = Tetronimo::Rotation::North;
		break;
	case 'e': rot = Tetronimo::Rotation::East;
		break;
	case 's': rot = Tetronimo::Rotation::South;
		break;
	case 'w': rot = Tetronimo::Rotation::West;
		break;
	default: rot = rot;
	}
	return rot;
}
char fromTetroRotToChar(const tetris_clone::Tetronimo::Rotation r) {
	char res;
	switch (r) {
		using namespace tetris_clone;
	case Tetronimo::Rotation::North: res = 'n';
		break;
	case Tetronimo::Rotation::East: res = 'e';
		break;
	case Tetronimo::Rotation::South: res = 's';
		break;
	case Tetronimo::Rotation::West: res = 'w';
		break;
	default: res = '\0';
	}
	return res;
}
char fromTetroToChar(const tetris_clone::Tetronimo::Flavor f) {
	char result{};
	switch(f) {
		using namespace tetris_clone;
	case Tetronimo::Flavor::S: result = 'S';
		break;
	case Tetronimo::Flavor::Z: result = 'Z';
		break;
	case Tetronimo::Flavor::T: result = 'T';
		break;
	case Tetronimo::Flavor::J: result = 'J';
		break;
	case Tetronimo::Flavor::L: result = 'L';
		break;
	case Tetronimo::Flavor::I: result = 'I';
		break;
	case Tetronimo::Flavor::O: result = 'O';
		break;
	default: result = '\0';
	}
	return result;
}
#endif

MacroManager::MacroManager() : fileHandler_{}, openFile_{} {}

std::vector<MacroManager::tetr_optional_t> MacroManager::openMacro(const char* pathname) {
	fileHandler_.startStream(false, pathname);
	std::vector <tetr_optional_t> res{};
	while (fileHandler_.hasMore()) {
		auto inputResult = fileHandler_.read();
		if (std::get<0>(inputResult) != 'A') {
			const auto tetrominoType = toTetro(std::get<0>(inputResult));
			const auto rotation = toTetroRot(std::get<1>(inputResult));
			res.emplace_back( std::in_place_t{}, tetrominoType, std::pair{ std::get<2>(inputResult), std::get<3>(inputResult) });
			res.back()->setRotation(rotation);
		}
		else {
			res.emplace_back();
		}
	}
	return res;
}

bool MacroManager::writeMacro(const std::vector<tetr_optional_t>& tetros, const char* pathname) {
	return false;
}