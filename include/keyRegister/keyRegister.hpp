#ifndef KEYREGISTER_HPP_
# define KEYREGISTER_HPP_

#include "spider/packetSerializer.hpp"

#include <list>
#include <iostream>
#include <fstream>


namespace spider
{
	class KeyRegister
	{
	public:
		KeyRegister(size_t fileMaxSize = 1000000);
		void write(char *msg, int length);
		char *read();

	private:
		void swapMode();
		void openReadableFile();
		void openWriteableFile();
		std::string getRandomAscii();

	private:
		std::fstream			_currentFile;
		bool					_writeMode;
		std::list<std::string>	_file;
		size_t					_currentFileSize;
		size_t 					_fileMaxSize;
	};
}

#endif /* KEYREGISTER_HPP_ */