#include "keyRegister/keyRegister.hpp"
#include <stdlib.h>
#include <time.h>

KeyRegister::KeyRegister(size_t fileMaxSize)
	: _currrentFileSize(0), _fileMaxSize(fileMaxSize)
{
	_writeMode = true;
	srand (time(NULL));
}

char *KeyRegister::read()
{

}

void KeyRegister::swapMode()
{
	if (_writeMode)
	{
		_writeMode = false;

	}
	else
	{
		_writeMode = true;
	}
}

void openReadableFile()
{
	if (!_file.empty())
	{
		_currrentFileSize.open(_file.front(),std::ifstream::in | std::ifstream::binary)
	}
}

void openWriteableFile()
{
	_currrentFileSize.open(std::string("Winlog-" + ))
}

void getRandomAscii()
{
	std::string randomAscii;
	for (int i = 0; i < 10; ++i)
		randomAscii + rand() %90 + 32;
	std::cout << randomAscii << std::endl;
	return (randomAscii)
}