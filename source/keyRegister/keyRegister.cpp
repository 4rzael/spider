#include "keyRegister/keyRegister.hpp"

KeyRegister::KeyRegister(size_t fileMaxSize)
	: _currrentFileSize(0), _fileMaxSize(fileMaxSize)
{
	_writeMode = true;
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

}