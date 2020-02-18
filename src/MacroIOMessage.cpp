#include "MacroIOMessage.h"

#include <fstream>
#include <filesystem>
#include <string>

MacroIOMessage::MacroIOMessage() : _ioStream{}, _openFile{}, _lastCharWritten{} {
}

bool MacroIOMessage::startStream(bool writeMode, const char* name, int fileno) {
	_writeMode = writeMode;
	int nameLen{};

	for (int i = 0; name[i] != '\0' and nameLen < 11; ++i) {
		_name[i] = name[i];
		++nameLen;
	}
	_labelLen = nameLen;

	_name[nameLen] = '\0';

	char sequenceNo[3] = { {},{},{} };
	chooseFilename(fileno, sequenceNo);

	for (int i = 0; i < (sizeof(sequenceNo) / sizeof(char)); ++i)
		_name[nameLen + i] = sequenceNo[i];

	//_name[nameLen + (sizeof(sequenceNo) / sizeof(char))] = '\0';
	
	openFile();

	if (!_openFile)
		return false;
	return true;

}

void MacroIOMessage::chooseFilename(int fileno, char* name) {
	std::string attempt = std::string{} + _name + "00";

	if (!_writeMode and !(fileno < 0)) {
		int pos{};
		int size = 2;
		if (fileno < 10) {
			pos = 1;
			size = 1;
		}
		attempt.replace(_labelLen + pos, size, std::to_string(fileno));
		if (std::filesystem::exists(attempt)) {
			_openFile = true;
			name[0] = attempt[_labelLen];
			name[1] = attempt[_labelLen + 1];
		}
		else
		{
			name[0] = 'F';
			name[1] = 'F';
		}
		name[2] = '\0';
		return;
	}
	//if (!std::filesystem::exists(attempt)) {
	//	return;
	//}
		int nPermutations = 0;
		attempt[_labelLen] = '0';
		for (int i = 0; i < 10; ++i) {
			attempt[_labelLen+1] = std::to_string(i)[0];
			if (!std::filesystem::exists(attempt)) {
				break;
			}
			++nPermutations;
		}
		if (nPermutations == 0 and !_writeMode) {
			name[0] = 'F';
			name[1] = 'F';
			name[2] = '\0';
			return;
		}
		attempt.replace(_labelLen+1, 1, std::to_string(nPermutations));
		if (nPermutations == 10) {
			while (nPermutations < 100 and std::filesystem::exists(attempt)) {
				++nPermutations;
				attempt.replace(_labelLen, 2, std::to_string(nPermutations));
			}
		}
		if (!_writeMode) {
			--nPermutations;
			if (nPermutations < 10)
				attempt[_labelLen + 1] = std::to_string(nPermutations)[0];
			else
				attempt.replace(_labelLen, 2, std::to_string(nPermutations));
		}
		name[0] = attempt[_labelLen];
		name[1] = attempt[_labelLen + 1];
		name[2] = '\0';
		_openFile = true;
}

void MacroIOMessage::openFile() {
	if (!_ioStream)
		_ioStream.emplace();
	if (_openFile) {
		if (_writeMode)
			_ioStream->open(_name, std::ios_base::out);
		else
			_ioStream->open(_name, std::ios_base::in);
	}
}

bool MacroIOMessage::write(char c, char r, int x, int y) {
	if (!_writeMode)
		return false;

	if (c == 'A' and _lastCharWritten == 'A') { return true; }
	*_ioStream << c << ' ' << r << ' ' << x << ' ' << y << '\n';

	_lastCharWritten = c;

	return true;
}

bool MacroIOMessage::close() {
	_ioStream->close();
	_openFile = false;
	return true;
}

std::tuple<char, char, int, int> MacroIOMessage::read() {
	if (_writeMode or !_openFile)
		return std::make_tuple('\0', '\0', -1, -1);
	std::tuple<char, char, int, int> inputValue{};

	*_ioStream >> std::get<0>(inputValue) >> std::get<1>(inputValue) >> std::get<2>(inputValue) >> std::get<3>(inputValue) >> std::ws;

	return inputValue;
}

bool MacroIOMessage::hasMore() {
	if (!_openFile or _writeMode)
		return false;
	return !_ioStream->eof();
}
