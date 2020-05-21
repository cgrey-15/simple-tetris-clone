#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H

#include "MacroIOMessage.h"
#include "tetromino/Tetronimo.h"
#include <vector>
#include <optional>

tetris_clone::Tetronimo::Flavor toTetro(const char c);
tetris_clone::Tetronimo::Rotation toTetroRot(const char c);
char fromTetroToChar(const tetris_clone::Tetronimo::Flavor f);
char fromTetroRotToChar(const tetris_clone::Tetronimo::Rotation r);
char fromTetroToChar(const tetris_clone::Tetronimo::Flavor f);

class MacroManager {
	using tetr_optional_t = std::optional<tetris_clone::Tetronimo>;
public:

	explicit MacroManager();
	std::vector <tetr_optional_t> openMacro(const char* in);
	bool writeMacro(const std::vector <tetr_optional_t>& tetros, const char* out);

private:
	MacroIOMessage fileHandler_;
	bool openFile_;
};

#endif // MACRO_MANAGER_H
