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
		~KeyRegister();
		void write(char *msg, int length);
		int read(char *&messages);
		void swapMode();

	private:
		void openReadableFile();
		void openWriteableFile();
		void getRandomAscii(std::string &filname);

	private:
		std::fstream			_currentFile;
		bool					_writeMode;
		std::list<std::string>	_file;
		size_t					_currentFileSize;
		size_t 					_fileMaxSize;
	};
}

#endif /* KEYREGISTER_HPP_ */