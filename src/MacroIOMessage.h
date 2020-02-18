#ifndef MACRO_IO_MESSAGE_H
#define MACRO_IO_MESSAGE_H

#include <iosfwd>
#include <tuple>
#include "Corrade/Containers/Pointer.h"

class MacroIOMessage {

	Corrade::Containers::Pointer<std::fstream> _ioStream;
	bool _writeMode;
	char _name[14];
	int _labelLen;
	bool _openFile;
	char _lastCharWritten;

public:
	MacroIOMessage();

	bool startStream(bool writeMode, const char* name, int fileno = -1);
	bool write(char c, char r, int x, int y);
	bool close();
	std::tuple<char, char, int, int> read();
	bool hasMore();

private:
	void openFile();
	void chooseFilename(int fileno, char* name);
};

#endif